using System;
using System.Collections;
using System.Collections.Generic;
using Softing.OPCToolbox.Client;
using Softing.OPCToolbox;

namespace AEConsole
{
	public class MyAeSubscription : AeSubscription
	{

		#region Constructor
		//-----------------

		public MyAeSubscription(MyAeSession parentSession) : base(parentSession)
		{
			PerformStateTransitionCompleted += new PerformStateTransitionEventHandler(HandlePerformObjectStateTransitionCompleted);
			StateChangeCompleted += new StateChangeEventHandler(HandleStateChangeCompleted);
			AeEventsReceived += new AeEventsReceivedEventHandler(HandleEventsReceived);
			AeConditionsChanged += new AeConditionsChangedEventHandler(HandleConditionsChanged);
		}

		//--
		#endregion

		#region Private Members
		//---------------------

		#region Private Attributes
		//------------------------


		//--
		#endregion


		//--
		#endregion

		#region Public Methods
		//---------------------

		// creates the string representation of the AeCondition.State property
		public static string GetState(EnumConditionState state)
		{
			string stateToString = string.Empty;
			if ((state & EnumConditionState.ACKED) == EnumConditionState.ACKED)
			{
				stateToString += "ACKED ";
			}
			if ((state & EnumConditionState.ACTIVE) == EnumConditionState.ACTIVE)
			{
				stateToString += "ACTIVE ";
			}
			if ((state & EnumConditionState.ENABLED) == EnumConditionState.ENABLED)
			{
				stateToString += "ENABLED ";
			}
			return stateToString;
		}// end GetState

		// creates the string representation of the AeCondition.ChangeMask property
		public static string GetWhatChanged(EnumConditionChange change)
		{
			string changeToString = string.Empty;
			if ((change & EnumConditionChange.ACK_DATA) == EnumConditionChange.ACK_DATA)
			{
				changeToString += "ACK_DATA ";
			}
			if ((change & EnumConditionChange.ACK_STATE) == EnumConditionChange.ACK_STATE)
			{
				changeToString += "ACK_STATE ";
			}
			if ((change & EnumConditionChange.ACTIVE_STATE) == EnumConditionChange.ACTIVE_STATE)
			{
				changeToString += "ACTIVE_STATE ";
			}
			if ((change & EnumConditionChange.ATTRIBUTE) == EnumConditionChange.ATTRIBUTE)
			{
				changeToString += "ATTRIBUTE ";
			}
			if ((change & EnumConditionChange.ENABLE_STATE) == EnumConditionChange.ENABLE_STATE)
			{
				changeToString += "ENABLE_STATE ";
			}
			if ((change & EnumConditionChange.MESSAGE) == EnumConditionChange.MESSAGE)
			{
				changeToString += "MESSAGE ";
			}
			if ((change & EnumConditionChange.QUALITY) == EnumConditionChange.QUALITY)
			{
				changeToString += "QUALITY ";
			}
			if ((change & EnumConditionChange.SEVERITY) == EnumConditionChange.SEVERITY)
			{
				changeToString += "SEVERITY ";
			}
			if ((change & EnumConditionChange.STATE) == EnumConditionChange.STATE)
			{
				changeToString += "STATE ";
			}
			if ((change & EnumConditionChange.SUBCONDITION) == EnumConditionChange.SUBCONDITION)
			{
				changeToString += "SUBCONDITION ";
			}
			return changeToString;
		}// end GetWhatChanged

		//--
		#endregion

		#region Public Properties
		//-----------------------


		//--
		#endregion

		#region Handles

		// method that handles the completion of performing the ObjectSpaceElement state transition
		public static void HandlePerformObjectStateTransitionCompleted(ObjectSpaceElement obj, uint executionContext, int result)
		{
			if (ResultCode.SUCCEEDED(result))
			{
				System.Console.WriteLine(obj.ToString() + " Performed state transition " + executionContext);
			}
			else
			{
				System.Console.WriteLine(obj.ToString() + " Performed state transition failed");
			}
		}

		// method that handles the completion of state changing of an ObjectSpaceElement
		public static void HandleStateChangeCompleted(ObjectSpaceElement obj, EnumObjectState state)
		{
			System.Console.WriteLine(obj + " State Changed - " + state);
		}

		// method that handles the AeSubscription.AeEventsReceived event; it displays on the console the received events
		public static void HandleEventsReceived(AeSubscription anAeSubscription, bool refresh, bool lastRefresh, AeEvent[] events)
		{
			System.Console.WriteLine();
			System.Console.WriteLine(anAeSubscription.ToString() + "	Events Received:" + events.Length);
			for (int i = 0; i < events.Length; i++)
			{
				if ((events[i].EventType == EnumEventType.SIMPLE) || (events[i].EventType == EnumEventType.TRACKING))
				{
					System.Console.WriteLine("	Event type: " + events[i].EventType);
					System.Console.WriteLine("	Event category: " + events[i].Category);
					System.Console.WriteLine("	Source path: " + events[i].SourcePath);
					System.Console.WriteLine("	Message: " + events[i].Message);
					System.Console.WriteLine("	Occurence time: " + events[i].OccurenceTime.ToString());
					System.Console.WriteLine("	Severity: " + events[i].Severity);
					System.Console.WriteLine("	Actor id: " + events[i].ActorId);
					ArrayList attributes = events[i].Attributes;
					System.Console.WriteLine("     Attributes: " + attributes.Count);
					for (int j = 0; j < events[i].Attributes.Count; j++)
					{
						System.Console.WriteLine(events[i].Attributes[j]);
					}
				}//end if
				if (events[i].EventType == EnumEventType.CONDITION)
				{
					System.Console.WriteLine("	Event type: " + events[i].EventType);
					System.Console.WriteLine("	Event category: " + events[i].Category);
					System.Console.WriteLine("	Source path: " + events[i].SourcePath);
					System.Console.WriteLine("	Message: " + events[i].Message);
					System.Console.WriteLine("	Occurence time: " + events[i].OccurenceTime.ToString());
					System.Console.WriteLine("	Severity: " + events[i].Severity);
					System.Console.WriteLine("	Actor id: " + events[i].ActorId);
					ArrayList attributes = events[i].Attributes;
					System.Console.WriteLine("     Attributes: " + attributes.Count);
					for (int j = 0; i < events[i].Attributes.Count; j++)
					{
						System.Console.WriteLine(events[i].Attributes[j]);
					}
					System.Console.WriteLine("	Change mask: " + GetWhatChanged(((AeCondition)events[i]).ChangeMask));
					string state = String.Empty;
					System.Console.WriteLine("	New state: " + GetState(((AeCondition)events[i]).State));
					System.Console.WriteLine("	Ack Required: " + ((AeCondition)events[i]).AckRequired);
					System.Console.WriteLine("	Quality: " + ((AeCondition)events[i]).Quality);
					System.Console.WriteLine("	Condition name: " + ((AeCondition)events[i]).ConditionName);
					System.Console.WriteLine("	Sub condition name: " + ((AeCondition)events[i]).SubConditionName);
					System.Console.WriteLine("	Active time: " + ((AeCondition)events[i]).ActiveTime.ToString());

					string[] sourceConditions;
					int res = anAeSubscription.AeSession.QueryAeSourceConditions(events[i].SourcePath, out sourceConditions, new ExecutionOptions());   
					if (ResultCode.SUCCEEDED(res))
					{
						System.Console.WriteLine("Source conditions for "+ events[i].SourcePath);
						foreach(string source in sourceConditions)
						{
							System.Console.WriteLine(source);
						}
					}
					else
					{
						System.Console.WriteLine("Error while querying source conditions for"+ events[i].SourcePath + " : " + res);
					}
				}//end if
			}//end  for			
		}

		// method that handles the AeSubscription.AeConditionsChanged event
		public static void HandleConditionsChanged(AeSubscription anAeSubscription, AeCondition[] conditions)
		{
			System.Diagnostics.Debug.WriteLine("Conditions changed");
		}

		//--
		#endregion

	}   //	end class MyAeItem

}   //	end namespace
