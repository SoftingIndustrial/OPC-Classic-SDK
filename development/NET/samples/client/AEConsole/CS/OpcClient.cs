using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace AEConsole
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

		private string GetStateToString(EnumConditionState state)
		{
			string stateToString = string.Empty;

			if ((state & EnumConditionState.ACTIVE) == EnumConditionState.ACTIVE)
			{
				stateToString += " ACT";
			}   //    end if
			if ((state & EnumConditionState.ENABLED) == EnumConditionState.ENABLED)
			{
				stateToString += " ENA";
			}   //    end if
			if ((state & EnumConditionState.ACKED) == EnumConditionState.ACKED)
			{
				stateToString += " ACK";
			}   //    end if
			if (state == 0)
			{
				stateToString += " DIS";
			}   //    end if
			return stateToString;
		}   //    end StateToString

		//--
		#endregion

		//--
		#endregion

		#region Public Methods
		//---------------------

		public Application GetApplication()
		{
			return Application.Instance;
		}   //    end GetApplication

		public int Initialize()
		{

			int result = (int)EnumResultCode.S_OK;
			GetApplication().VersionOtb = 447;
			//    TODO - binary license activation
			//    Fill in your binary license activation keys here
			//
			//    NOTE: you can activate one or all of the features at the same time

			//    activate the COM-AE Client Feature
			//    result = Application.Instance.Activate(EnumFeature.AE_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}
			//    END TODO - binary license activation

			//    proceed with the OPC Toolkit core initialization
			result = GetApplication().Initialize();

			if (ResultCode.SUCCEEDED(result))
			{
				//    enable toolkit internal initialization
				GetApplication().EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.CLIENT,
					EnumTraceGroup.CLIENT,
					"Trace.txt",
					1000000,
					0);
			}   //    end if
			return result;
		}   //    end Initialize

		public int ProcessCommandLine(string commandLine)
		{
			//    forward the command line arguments to the OPC Toolkit core internals
			return Application.Instance.ProcessCommandLine(commandLine);
		}   //    end ProcessCommandLine


		public void Terminate()
		{
			// disconnect all the connected objects
			if (m_aeSubscription.CurrentState != EnumObjectState.DISCONNECTED)
			{
				m_aeSubscription.Disconnect(new ExecutionOptions());
			}

			if (m_aeSession.CurrentState != EnumObjectState.DISCONNECTED)
			{
				m_aeSession.Disconnect(new ExecutionOptions());
			}

			// remove subscription from session
			m_aeSession.RemoveAeSubscription(m_aeSubscription);

			// remove session from application
			GetApplication().RemoveAeSession(m_aeSession);

			// terminate the application
			GetApplication().Terminate();
			m_aeSession = null;
			m_aeSubscription = null;
			m_executionOptions = null;
		}   //    end Terminate


		public string GetConditionState()
		{
			if (m_aeSession == null)
			{
				return "";
			}   //    end if

			string message = String.Empty;

			try
			{
				string conditionStateToString = String.Empty;
				AeConditionState conditionState = null;

				int result = m_aeSession.GetAeConditionState(
								"computer.clock.time slot 1",
								"between",
								null,
								out conditionState,
								null);

				if (ResultCode.SUCCEEDED(result))
				{

					message = "The condition state is: \n";
					message += " Source Path: computer.clock.time slot 1 \n";
					message += " Condition Name: between \n";
					message += " State: " + GetStateToString(conditionState.State) + "\n";
					message += " Quality: " + conditionState.Quality + "\n";
					message += " Active Time: " + conditionState.ConditionActiveTime + "\n";
					message += " Inactive Time: " + conditionState.ConditionInactiveTime + "\n";
					message += " AcknowledgeTime: " + conditionState.ConditionAckTime + "\n";
					message += " AcknowledgerComment: " + conditionState.AcknowledgerComment + "\n";
					message += " AcknowledgerID: " + conditionState.AcknowledgerId + "\n";
					message += " Active subcondition time: " + conditionState.SubConditionActiveTime + "\n";
					message += " Active subcondition name: " + conditionState.ActiveSubConditionName + "\n";
					message += " Active subcondition definition: " + conditionState.ActiveSubConditionDefinition + "\n";
					message += " Active subcondition description: " + conditionState.ActiveSubConditionDescription + "\n";
					message += " Active subcondition severity: " + conditionState.ActiveSubConditionSeverity + "\n";
					message += " Number of subconditions: " + conditionState.SubConditionsNames.Length + "\n";
					for (int i = 0; i < conditionState.SubConditionsNames.Length; i++)
					{
						message += "	Subcondition name: " + conditionState.SubConditionsNames[i] + "\n";
						message += "	Subcondition definition: " + conditionState.SubConditionsDefinitions[i] + "\n";
						message += "	Subcondition description: " + conditionState.SubConditionsDescriptions[i] + "\n";
						message += "	Subcondition severity: " + conditionState.SubConditionsSeverities[i] + "\n";
					}//end for
				}
				else
				{
					message = "Get condition state failed!\n";
				}   //    end if...else
			}
			catch (Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::GetConditionState",
					exc.ToString());
			}   //    end try...catch
			return message;
		}//end GetConditionState

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

				connectResult = m_aeSession.Connect(true, false, new ExecutionOptions());

				//define the event areas that will be used to filter events
				//TODO replace the array below with your own areas
				// NOTE: Areas excluded from this filter will not send events/conditions to the AE client.
				//       If no filter is set, all areas will fire events.
				string[] areas = new string[] { "computer.mouse", "computer.clock" };

				//set the previously defined areas for filtering
				m_aeSubscription.FilterAreas = areas;

				//define the event sources that will be used to filter events
				//TODO replace the array below with your own sources
				// NOTE: Sources excluded from this filter will not send events/conditions to the AE client.
				//       If no filter is set, all sources will fire events.
				string[] sources = new string[] { "computer.mouse.right button", "computer.mouse.middle button", "computer.clock.timer", "computer.clock.time slot 1", "computer.clock.time slot 2" };

				//set the previously defined sources for filtering
				m_aeSubscription.FilterSources = sources;

				//define the categories that will be used to filter events ("time tick" category is used)
				//TODO replace the array below with your own category ids
				// NOTE: Category IDs excluded from this filter will not send events/conditions to the AE client.
				//       If no filter is set, all categories will fire events.
				uint[] categoryIds = new uint[] { 1, 2, 3 };

				//set the previously defines categoryIds for filtering
				m_aeSubscription.FilterCategories = categoryIds;

				// NOTE: Event/conditions property IDs not included in this filter will not be displayed.
				//       If no property filter is set, NO PROPERTIES WILL BE SENT WITH EACH EVENT/CONDITION (except for standard AE event/condition properties).
				AeReturnedAttributes[] returnedAttributes = new AeReturnedAttributes[3];

				// category ID: 1 (time tick)
				// attribute IDs: 1 (second)
				uint[] attributeIds = new uint[1];
				attributeIds[0] = 1;
				returnedAttributes[0] = new AeReturnedAttributes();
				returnedAttributes[0].AttributeIds = attributeIds;
				returnedAttributes[0].CategoryId = 1;

				// category ID: 2 (time slot)
				// attribute IDs: 2 (second), 3 (start second), 4 (end second)
				attributeIds = new uint[3];
				attributeIds[0] = 2;
				attributeIds[1] = 3;
				attributeIds[2] = 4;
				returnedAttributes[1] = new AeReturnedAttributes();
				returnedAttributes[1].AttributeIds = attributeIds;
				returnedAttributes[1].CategoryId = 2;

				// category ID: 3 (mouse click)
				// attribute IDs: 5 (x position), 6 (y position)
				attributeIds = new uint[2];
				attributeIds[0] = 5;
				attributeIds[1] = 6;
				returnedAttributes[2] = new AeReturnedAttributes();
				returnedAttributes[2].AttributeIds = attributeIds;
				returnedAttributes[2].CategoryId = 3;

				m_aeSubscription.ReturnedAttributes = returnedAttributes;

			}
			catch (Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::InitializeAeObjects",
					exc.ToString());
			}   //    end try...catch

			return connectResult;
		}   //    end InitializeAeObjects

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
		}   //    end Trace

		public void activateObjectsAsync()
		{
			System.Console.WriteLine();
			// activate the session asynchronously
			m_aeSession.Connect(true, true, m_executionOptions);
			// increment the execution context used to identify the callback that comes as response
			m_executionOptions.ExecutionContext++;
		}// end activateObjectsAsync

		public void activateObjectsSync()
		{
			System.Console.WriteLine();
			// activate the session synchronously
			m_aeSession.Connect(true, true, new ExecutionOptions());
		}// end activateObjectsSync

		public void connectObjectsAsync()
		{
			System.Console.WriteLine();
			// connect the session asynchronously
			m_aeSession.Connect(true, false, m_executionOptions);
			// increment the execution context used to identify the callback that comes as response
			m_executionOptions.ExecutionContext++;
		}// end connectObjectsAsync

		public void connectObjectsSync()
		{
			System.Console.WriteLine();
			// connect the session synchronously
			m_aeSession.Connect(true, false, new ExecutionOptions());
		}// end connectObjectsSync

		public void disconnectObjectsAsync()
		{
			System.Console.WriteLine();
			// disconnect the session asynchronously
			m_aeSession.Disconnect(m_executionOptions);
			// increment the execution context used to identify the callback that comes as response
			m_executionOptions.ExecutionContext++;
		}// end disconnectObjectsAsync

		public void disconnectObjectsSync()
		{
			// disconnect the session synchronously
			System.Console.WriteLine();
			m_aeSession.Disconnect(new ExecutionOptions());
		}// end disconnectObjectsSync

		public void getServerStatusAsync()
		{
			ServerStatus newServerStatus;
			// get the session status asynchronously
			int statusRes = m_aeSession.GetStatus(out newServerStatus, m_executionOptions);
			// increment the execution context used to identify the callback that comes as response
			m_executionOptions.ExecutionContext++;
		}// end getServerStatusAsync

		public void getServerStatusSync()
		{
			ServerStatus serverStatus;
			// get the session status synchronously
			if (ResultCode.SUCCEEDED(m_aeSession.GetStatus(out serverStatus, new ExecutionOptions())))
			{

				System.Console.WriteLine(" Server Status");
				System.Console.WriteLine("	Vendor info: " + serverStatus.VendorInfo);
				System.Console.WriteLine("	Product version: " + serverStatus.ProductVersion);
				System.Console.WriteLine("	State: " + serverStatus.State);
				System.Console.WriteLine("	Start time: " + serverStatus.StartTime);
				System.Console.WriteLine("	Last update time: " + serverStatus.LastUpdateTime);
				System.Console.WriteLine("	Current time: " + serverStatus.CurrentTime);
				System.Console.WriteLine("	GroupCount: " + serverStatus.GroupCount);
				System.Console.WriteLine("	Bandwidth: " + serverStatus.Bandwidth);
				for (int i = 0; i < serverStatus.SupportedLcIds.Length; i++)
				{

					System.Console.WriteLine("	Supported LCID: " + serverStatus.SupportedLcIds[i]);
				}
				System.Console.WriteLine("	Status info: " + serverStatus.StatusInfo);
			}
			else
			{
				System.Console.WriteLine(" Get Status failed ");
			}
		}// end getServerStatusSync

		public void activateConnectionMonitor()
		{
			// activate the monitor that watches the connection status
			if (ResultCode.SUCCEEDED(m_aeSession.ActivateConnectionMonitor(true, 5000, 0, 10000, 300000)))
			{
				System.Console.WriteLine();
				System.Console.WriteLine("Activated connection monitor");
				System.Console.WriteLine();
			}
			else
			{
				System.Console.WriteLine("Activate connection monitor failed");
			}
		}// end activateConnectionMonitor

		public void deactivateConnectionMonitor()
		{
			// deactivate the monitor that watches the connection status
			if (ResultCode.SUCCEEDED(m_aeSession.ActivateConnectionMonitor(false, 0, 0, 0, 0)))
			{
				System.Console.WriteLine();
				System.Console.WriteLine("Deactivated connection monitor");
				System.Console.WriteLine();
			}
			else
			{
				System.Console.WriteLine("Deactivate connection monitor failed");
			}
		}// end deactivateConnectionMonitor


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
		}   //    end SetServiceName

		//--
		#endregion

	}   //    end class OpcClient

}   //    end namespace
