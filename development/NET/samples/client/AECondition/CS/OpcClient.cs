using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace AECondition
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
		private OpcForm m_opcForm = null;
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
			}   //	end if
			if ((state & EnumConditionState.ENABLED) == EnumConditionState.ENABLED)
			{
				stateToString += " ENA";
			}   //	end if
			if ((state & EnumConditionState.ACKED) == EnumConditionState.ACKED)
			{
				stateToString += " ACK";
			}   //	end if
			if (state == 0)
			{
				stateToString += " DIS";
			}   //	end if
			return stateToString;
		}   //	end StateToString

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
			}   //	end if

			if (m_aeSession.CurrentState != EnumObjectState.DISCONNECTED)
			{
				m_aeSession.Disconnect(m_executionOptions);
			}   //	end if

			m_aeSession.RemoveAeSubscription(m_aeSubscription);
			GetApplication().RemoveAeSession(m_aeSession);

			GetApplication().Terminate();
			m_aeSession = null;
			m_aeSubscription = null;
			m_executionOptions = null;
		}   //	end Terminate


		public string GetConditionState()
		{
			if (m_aeSession == null)
			{
				return "";
			}   //	end if

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
				}   //	end if...else
			}
			catch (Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::GetConditionState",
					exc.ToString());
			}   //	end try...catch
			return message;
		}//end GetConditionState

		public int InitializeAeObjects()
		{
			int connectResult = (int)EnumResultCode.E_FAIL;
			int result = (int)EnumResultCode.S_OK;

			m_executionOptions = new ExecutionOptions();

			try
			{

				m_aeSession = new MyAeSession("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}");
				m_aeSubscription = new MyAeSubscription(m_aeSession);

				connectResult = m_aeSession.Connect(
					true,
					true,
					m_executionOptions);

				result = m_aeSubscription.RefreshAeConditions(m_executionOptions);

				if (!ResultCode.SUCCEEDED(result))
				{
					string errorText = String.Empty;
					m_aeSession.GetErrorString(result, out errorText, m_executionOptions);
					System.Diagnostics.Debug.WriteLine(errorText);
				} //end if		
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

		public void SetForm(OpcForm form)
		{
			m_opcForm = form;
			m_aeSession.SetForm(m_opcForm);
			m_aeSubscription.SetForm(m_opcForm);
		}

		public ExecutionOptions GetExecutionOptions()
		{
			return m_executionOptions;
		}

		public MyAeSession GetSession()
		{
			return m_aeSession;
		}

		public MyAeSubscription GetSubscription()
		{
			return m_aeSubscription;
		}

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
