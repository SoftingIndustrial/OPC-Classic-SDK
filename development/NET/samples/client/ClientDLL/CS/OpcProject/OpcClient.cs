using System;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace TestDll
{
	public class OpcClient
	{
	
		#region Constructor
		//-----------------

		public OpcClient(){}

		//--
		#endregion

		#region Private Members
		//---------------------

		#region Private Attributes
		//------------------------

		private DaSession m_daSession = null;
		private DaSubscription m_daSubscription = null;
		private DaItem m_daItem = null;
		private AeSession m_aeSession = null;
		private AeSubscription m_aeSubscription = null;
		//--
		#endregion

		#region Private Methods
		//---------------------

		private string GetStateToString(EnumConditionState state)
		{
			string stateToString = string.Empty;

			if((state & EnumConditionState.ACTIVE) == EnumConditionState.ACTIVE)
			{
				stateToString += " ACT";
			}	//	end if
			if((state & EnumConditionState.ENABLED) == EnumConditionState.ENABLED)
			{
				stateToString += " ENA";
			}	//	end if
			if((state & EnumConditionState.ACKED) == EnumConditionState.ACKED)
			{
				stateToString += " ACK";
			}	//	end if
			if(state == 0)
			{
				stateToString += " DIS";
			}	//	end if
			return stateToString;
		}	//	end StateToString

		//--
		#endregion

		//--
		#endregion
		
		#region Public Methods
		//---------------------

		public Application GetApplication()
		{
			return Application.Instance;
		}	//	end GetApplication

		public int Initialize()
		{
			GetApplication().VersionOtb = 447;
			int result = GetApplication().Initialize();
			if (ResultCode.SUCCEEDED(result))
			{
				GetApplication().EnableTracing(					
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.CLIENT,
					EnumTraceGroup.CLIENT,
					"Trace.txt",					
					1000000,
					2);
			}	//	end if
			return result;
		}	//	end Initialize

		public int ProcessCommandLine(string commandLine)
		{
			return Application.Instance.ProcessCommandLine(commandLine);
		}	//	end ProcessCommandLine


		public void Terminate()
		{
			GetApplication().Terminate();
			m_daSession = null;
			m_daSubscription = null;
			m_daItem = null;
			m_aeSession = null;
			m_aeSubscription = null;
		}	//	end Terminate


		public string GetConditionState()
		{
			if (m_aeSession == null)
			{
				return "";
			}	//	end if

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
				
				if (ResultCode.SUCCEEDED(result)){

					message = "The condition state is: \n";
					message += " Source Path: computer.clock.time slot 1 \n";
					message += " Condition Name: between \n";
					message += " State: " + GetStateToString(conditionState.State)+"\n";
					message += " Quality: " + conditionState.Quality +"\n";
					message += " Active Time: " + conditionState.ConditionActiveTime +"\n";
					message += " Inactive Time: " + conditionState.ConditionInactiveTime +"\n";
					message += " AcknowledgeTime: " + conditionState.ConditionAckTime +"\n";
					message += " AcknowledgerComment: " + conditionState.AcknowledgerComment +"\n";
					message += " AcknowledgerID: " + conditionState.AcknowledgerId +"\n";
					message += " Active subcondition time: " + conditionState.SubConditionActiveTime +"\n";
					message += " Active subcondition name: " + conditionState.ActiveSubConditionName +"\n";
					message += " Active subcondition definition: " + conditionState.ActiveSubConditionDefinition +"\n";
					message += " Active subcondition description: " + conditionState.ActiveSubConditionDescription +"\n";
					message += " Active subcondition severity: " + conditionState.ActiveSubConditionSeverity +"\n";
					message += " Number of subconditions: " + conditionState.SubConditionsNames.Length +"\n";
					for (int i = 0; i < conditionState.SubConditionsNames.Length; i++)
					{
						message += "	Subcondition name: " + conditionState.SubConditionsNames[i]+"\n";
						message += "	Subcondition definition: " + conditionState.SubConditionsDefinitions[i]+"\n";
						message += "	Subcondition description: " + conditionState.SubConditionsDescriptions[i]+"\n";
						message += "	Subcondition severity: " + conditionState.SubConditionsSeverities[i]+"\n";
					}//end for
				}
				else
				{
					message = "Get condition state failed!\n";
				}	//	end if...else
			}
			catch (Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::GetConditionState",
					exc.ToString());
			}	//	end try...catch
			return message;
		}//end GetConditionState

		public int InitializeAeObjects()
		{
			int connectResult = (int)EnumResultCode.E_FAIL;

			try{
				
				m_aeSession = new AeSession("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}");
				m_aeSubscription = new AeSubscription(m_aeSession);				

				connectResult = m_aeSession.Connect(
					true,
					true,
					null);

				m_aeSubscription.RefreshAeConditions(null);
			}
			catch(Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::InitializeAeObjects",
					exc.ToString());
			}	//	end try...catch

			return connectResult;
		}	//	end InitializeAeObjects
		public int InitializeDaObjects()
		{
			int connectResult = (int)EnumResultCode.E_FAIL;

			try{
				m_daSession = new DaSession("opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}");
				m_daSubscription = new DaSubscription(1000, m_daSession);
				m_daItem = new DaItem("maths.sin", m_daSubscription);

				connectResult = m_daSession.Connect(
					true,
					false,
					null);
			}
			catch(Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::InitializaDaObjects",
					exc.ToString());
			}	//	end try...catch

			return connectResult;
		}	//	end InitializeDaObjects

		public string ReadItem()
		{
			string message = String.Empty;

			try
			{
				uint readCount = 1;
				DaItem[] itemList = new DaItem[readCount];
				itemList[0] = m_daItem;

				ValueQT[] values = null;
				int[] results = null;

				if (ResultCode.SUCCEEDED(
					m_daSubscription.Read(
						0,
						itemList,
						out values,
						out results,
						null)))
				{
					message += " \nRead item synchronously using subscription \n";

					for (int i = 0; i< values.Length;i++)
					{
						if (ResultCode.SUCCEEDED(results[i]))
						{
							message += " " + itemList[i].Id + " - ";
							message += values[i].ToString() + "\n\n";
						}
						else{
							message += "Read failed for item " + itemList[i].Id + "\n\n";
						}	//	end if...else
					}	//	end for
				}
				else
				{
					message += " Subscription synchronous read failed!" + "\n\n";
				}	//	end if...else
			}
			catch(Exception exc)
			{
				GetApplication().Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER,
					"OpcClient::ReadItem",
					exc.ToString());
			}	//	end try...catch
			return message;
		}	//	end ReadItem


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
	  }	//	end Trace

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
		}	//	end SetServiceName

		//--
		#endregion

	}	//	end class OpcClient

}	//	end namespace
