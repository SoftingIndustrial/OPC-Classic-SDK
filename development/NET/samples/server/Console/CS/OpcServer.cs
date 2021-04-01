using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Console
{
	public class OpcServer
	{
		#region Constants
		//----------------
		private const uint CATEGORY_ID_SIMULATION = 1;
		private const uint ATTRIBUTE_ID_SIMULATION = 1;
		//--
		#endregion

		#region Constructor
		//-----------------
		public OpcServer()
		{

		}   //	end constructor
			//--
		#endregion

		#region Private Attributes
		//--------------------------

		private static AeCategory _timeTick = null;
		private static AeCategory _timeSlot = null;
		private AeCondition condition1 = null;
		private AeCondition condition2 = null;
		private ArrayList eventAttributes1 = null;
		private ArrayList eventAttributes2 = null;

		//--
		#endregion

		#region Protected Attributes
		//--------------------------

		protected ShutdownHandler m_shutdownRequest = null;
		protected MyDaAddressSpaceElement m_daSimulationElement = null;
		protected Random m_random = new Random(1);
		//--
		#endregion

		#region Public Attributes
		//--------------------------

		static public int g_angle = 0;
		static public MyDaAddressSpaceElement g_sin;
		static public MyDaAddressSpaceElement g_rand1s;
		static public MyDaAddressSpaceElement g_rand100ms;

		//--
		#endregion

		#region Public Constants
		//--------------------------

		public const string COND_NAME_BETWEEN_SINGLE = "between [10,50]";
		public const string COND_NAME_BETWEEN_MULTIPLE = "between";

		public const uint CAT_ID_TIME_TICK = 1;
		public const uint CAT_ID_TIME_SLOT = 2;

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

		#region Public Methods
		//-----------------------

		public static int Shutdown()
		{
			return (int)EnumResultCode.S_OK;
		}   //	end Shutdown


		public int Initialize()
		{
			try
			{
				Application.Instance.VersionOtb = 447;
				Application.Instance.AppType = EnumApplicationType.EXECUTABLE;
				Application.Instance.ClsidDa = "{B7502436-184A-4C93-BE7A-99989385FF1E}";
				Application.Instance.ProgIdDa = "Softing.OPC.Toolkit.NET.CS.Console.DA.1";
				Application.Instance.VerIndProgIdDa = "Softing.OPC.Toolkit.NET.CS.Console.DA";

				Application.Instance.ClsidAe = "{1C52A32D-2E2A-4248-9F4C-4E5676DF9CB1}";
				Application.Instance.ProgIdAe = "Softing.OPC.Toolkit.NET.CS.Console.AE.1";
				Application.Instance.VerIndProgIdAe = "Softing.OPC.Toolkit.NET.CS.Console.AE";

				Application.Instance.Description = "Softing OPC Toolkit .NET CS Console Sample Server";
				Application.Instance.ServiceName = string.Empty;
				Application.Instance.IpPortHTTP = 8079;
				Application.Instance.UrlDa = "/OPC/DA";
				Application.Instance.MajorVersion = 4;
				Application.Instance.MinorVersion = 47;
				Application.Instance.PatchVersion = 1;
				Application.Instance.BuildNumber = 0;
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH";
				Application.Instance.MinUpdateRateDa = 100;
				Application.Instance.ClientCheckPeriod = 30000;
				Application.Instance.AddressSpaceDelimiter = '/';
				Application.Instance.PropertyDelimiter = '#';
				Application.Instance.SupportDisableConditions = true;
				Application.Instance.WebRootDirectory = "www";
				Application.Instance.WebRootFile = string.Empty;
				Application.Instance.WebAdministratorPassword = "ad";
				Application.Instance.WebOperatorPassword = "op";

				Application.Instance.ShutdownRequest += new Softing.OPCToolbox.Server.ShutdownHandler(Shutdown);

			}
			catch (Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer::Initialize",
					exc.ToString());

				return (int)EnumResultCode.E_FAIL;
			}   //	end try...catch
			return (int)EnumResultCode.S_OK;
		}   //	end Initialize

		public int Start()
		{
			return Application.Instance.Start();
		}   //	end Start

		public int Stop()
		{
			return Application.Instance.Stop();
		}   //	end Stop

		public int Ready()
		{
			return Application.Instance.Ready();
		}   //	end Ready

		public int Terminate()
		{
			return Application.Instance.Terminate();
		}   //	end Terminate

		public int Prepare(MyCreator aMyCreator)
		{
			int result = (int)EnumResultCode.S_OK;
			//	TODO - binary license activation
			//	Fill in your binary license activation keys here
			//
			//	NOTE: you can activate one or all of the features at the same time

			//	activate the COM-DA Server feature
			//	result = Application.Instance.Activate(EnumFeature.DA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}

			//	activate the XML-DA Server Feature
			//	result = Application.Instance.Activate(EnumFeature.XMLDA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}

			//	activate the COM-AE Server Feature
			//	result = Application.Instance.Activate(EnumFeature.AE_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
			if (!ResultCode.SUCCEEDED(result))
			{
				return result;
			}
			//	END TODO - binary license activation

			result = Application.Instance.Initialize(aMyCreator);

			if (ResultCode.SUCCEEDED(result))
			{
				Application.Instance.EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.SERVER,
					EnumTraceGroup.SERVER,
					"Trace.txt",
					1000000,
					0);
			}   //	end if

			return result;
		}   //	end Prepare

		public int ProcessCommandLine(string commandLine)
		{
			return Application.Instance.ProcessCommandLine(commandLine);
		}   //	end ProcessCommandLine

		public int BuildAddressSpace()
		{
			try
			{
				MyDaAddressSpaceElement tag, node;

				tag = new MyDaAddressSpaceElement();
				tag.Name = "accept";
				tag.Type = MyDaAddressSpaceElement.TYPE_ACCEPT;
				tag.AccessRights = EnumAccessRights.READWRITEABLE;
				tag.Datatype = typeof(Int32);
				tag.IoMode = EnumIoMode.POLL;
				Application.Instance.DaAddressSpaceRoot.AddChild(tag);

				node = new MyDaAddressSpaceElement();
				node.Name = "maths";
				node.Type = MyDaAddressSpaceElement.TYPE_NODEMATH;
				node.IoMode = EnumIoMode.NONE;
				node.HasChildren = true;
				Application.Instance.DaAddressSpaceRoot.AddChild(node);

				g_sin = new MyDaAddressSpaceElement();
				g_sin.Name = "sin";
				g_sin.Type = MyDaAddressSpaceElement.TYPE_SINE;
				g_sin.AccessRights = EnumAccessRights.READABLE;
				g_sin.Datatype = typeof(double);
				g_sin.IoMode = EnumIoMode.REPORT;
				node.AddChild(g_sin);
				g_sin.SetEUInfoAnalog(-1.0, 1.0);


				DaProperty property = new DaProperty();
				property.Id = 6020;
				property.Name = "Angle";
				property.Description = "Angle Description";
				property.ItemId = property.Name;
				property.Datatype = typeof(short);
				property.AccessRights = EnumAccessRights.READWRITEABLE;
				g_sin.AddProperty(property);

				node = new MyDaAddressSpaceElement();
				node.Name = "random";
				node.Type = MyDaAddressSpaceElement.TYPE_NODERANDOM;
				node.IoMode = EnumIoMode.NONE;
				node.HasChildren = true;
				Application.Instance.DaAddressSpaceRoot.AddChild(node);

				g_rand100ms = new MyDaAddressSpaceElement();
				g_rand100ms.Name = "100ms";
				g_rand100ms.Type = MyDaAddressSpaceElement.TYPE_RANDOM_100MS;
				g_rand100ms.AccessRights = EnumAccessRights.READABLE;
				g_rand100ms.Datatype = typeof(Int32);
				g_rand100ms.IoMode = EnumIoMode.REPORT;
				node.AddChild(g_rand100ms);

				g_rand1s = new MyDaAddressSpaceElement();
				g_rand1s.Name = "1s";
				g_rand1s.Type = MyDaAddressSpaceElement.TYPE_RANDOM_1S;
				g_rand1s.AccessRights = EnumAccessRights.READABLE;
				g_rand1s.Datatype = typeof(Int32);
				g_rand1s.IoMode = EnumIoMode.REPORT;
				node.AddChild(g_rand1s);

				//	AE
				MyAeAddressSpaceElement computer = new MyAeAddressSpaceElement();
				computer.Name = "computer";
				computer.HasChildren = true;
				Application.Instance.AeAddressSpaceRoot.AddChild(computer);

				MyAeAddressSpaceElement clock = new MyAeAddressSpaceElement();
				clock.Name = "clock";
				clock.HasChildren = true;
				computer.AddChild(clock);

				MyAeAddressSpaceElement timer = new MyAeAddressSpaceElement();
				timer.Name = "timer";
				timer.HasChildren = false;
				clock.AddChild(timer);

				timer = new MyAeAddressSpaceElement();
				timer.Name = "time slot 1";
				timer.HasChildren = false;
				clock.AddChild(timer);

			}
			catch (Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer:BuildAddressSpace",
					exc.ToString());
				return (int)EnumResultCode.E_FAIL;
			}   //	end try...catch

			return (int)EnumResultCode.S_OK;
		}   //	end BuildAddressSpace


		public int BuildEventCategories()
		{
			int result = 0;
			try
			{
				_timeTick = new AeCategory(EnumEventType.SIMPLE, CAT_ID_TIME_TICK, "time tick");
				result = Application.Instance.AddAeCategory(_timeTick);

				if (ResultCode.SUCCEEDED(result))
				{
					_timeTick.AddAttribute(1, "hour", typeof(ushort));
					_timeTick.AddAttribute(2, "minute", typeof(ushort));
					_timeTick.AddAttribute(3, "second", typeof(ushort));
				}
				else
				{
					Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER, "Console.BuildEventCategories",
						"AddAeCategory has failed for <timeTick>");
				}   //	end if .. else

				_timeSlot = new AeCategory(EnumEventType.CONDITION, CAT_ID_TIME_SLOT, "time slot 1");
				result = Application.Instance.AddAeCategory(_timeSlot);
				if (ResultCode.SUCCEEDED(result))
				{
					_timeSlot.AddAttribute(1, "second", typeof(ushort));
					_timeSlot.AddConditionDefinition(COND_NAME_BETWEEN_SINGLE);
					_timeSlot.AddConditionDefinition(COND_NAME_BETWEEN_MULTIPLE);
					_timeSlot.AddSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, "[10,30[");
					_timeSlot.AddSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, "[30,50]");
				}
				else
				{
					Application.Instance.Trace(EnumTraceLevel.WRN, EnumTraceGroup.OPCSERVER, "Console.BuildEventCategories",
						"AddAeCategory has failed for <time slot 1>");
				}   //	end if ... else
			}
			catch (Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer::BuildEventCategories",
					exc.ToString());
				return (int)EnumResultCode.E_FAIL;
			}   //	end try...catch

			return (int)EnumResultCode.S_OK;

		}   //	end BuildEventCategories

		public int InitializeConditions()
		{
			try
			{
				condition1 = new AeCondition();
				condition2 = new AeCondition();

				eventAttributes1 = new ArrayList();
				eventAttributes1.Add(1);

				eventAttributes2 = new ArrayList();
				eventAttributes2.Add(2);

				condition1.Category = _timeSlot;
				condition1.Definition = _timeSlot.GetConditionDefinition(COND_NAME_BETWEEN_MULTIPLE);
				condition1.Name = COND_NAME_BETWEEN_MULTIPLE;

				condition1.SourcePath = "computer/clock/time slot 1";
				condition1.Quality = EnumQuality.GOOD;
				condition1.EventAttributes = eventAttributes1;
				condition1.UserData = 1;

				condition2.Category = _timeSlot;
				condition2.Name = COND_NAME_BETWEEN_SINGLE;
				condition2.Definition = _timeSlot.GetConditionDefinition(COND_NAME_BETWEEN_SINGLE);
				condition2.SourcePath = "computer/clock/time slot 1";
				condition2.Quality = EnumQuality.GOOD;
				condition2.EventAttributes = eventAttributes2;
				condition2.UserData = 2;

			}
			catch (Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer:initializeConditions",
					exc.ToString());
				return (int)EnumResultCode.E_FAIL;
			}   //	end try...catch

			return (int)EnumResultCode.S_OK;
		}// end InitializeConditions


		public void FireSimulationEvents()
		{
			//simple events
			ArrayList events = new ArrayList();
			AeEvent simulationEvent = new AeEvent(
				EnumEventType.SIMPLE,
				"SimulationElement",
				CATEGORY_ID_SIMULATION);

			simulationEvent.Severity = 10;
			simulationEvent.ActorId = "OpcUser";
			simulationEvent.Message = "Simulation event generated";
			events.Add(simulationEvent);

			Application.Instance.FireEvents(events);
		}   //	end FireEvents

		public void ChangeSimulationValues()
		{
			if (m_daSimulationElement != null)
			{
				m_daSimulationElement.ValueChanged(
					new ValueQT(
					m_random.Next(5000),
					EnumQuality.GOOD,
					DateTime.Now));

			}   //	end if
		}   //	end ChangeSimulationValues

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

		public void CheckValuesChanged(ref DateTime r_dt)
		{
			DateTime dateTimeNow = DateTime.UtcNow;
			Random rand = new Random();

			bool bSecChanged = r_dt.Second != dateTimeNow.Second;
			if (bSecChanged) { r_dt = dateTimeNow; }

			ArrayList changedElements = new ArrayList();
			ArrayList changedValues = new ArrayList();
			ValueQT val = null;
			val = new ValueQT(rand.Next(), EnumQuality.GOOD, DateTime.Now);
			changedElements.Add(g_rand100ms);
			changedValues.Add(val);

			if (bSecChanged)
			{
				val = new ValueQT(rand.Next(), EnumQuality.GOOD, DateTime.Now);
				changedElements.Add(g_rand1s);
				changedValues.Add(val);

				g_angle = ((++g_angle) % 360);
				val = new ValueQT(Math.Sin(g_angle / 180.0 * 3.14), EnumQuality.GOOD, DateTime.Now);
				changedElements.Add(g_sin);
				changedValues.Add(val);
			}

			Application.Instance.DaAddressSpaceRoot.ValuesChanged(changedElements, changedValues);

		} // end CheckValuesChanged

		public void AtSecond10(ref ArrayList r_conditionList)
		{
			condition1.Severity = 500;
			condition1.Message = "between [10,30]";
			condition1.AckRequired = true;
			condition1.ActiveSubConditionName = "[10,30]";
			condition1.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.SUBCONDITION |
				EnumConditionChange.ATTRIBUTE);

			condition1.StateChange = EnumConditionState.ACTIVE;

			condition2.Severity = 510;
			condition2.Message = "between [10,50]";
			condition2.AckRequired = true;
			condition2.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.ATTRIBUTE);
			condition2.StateChange = EnumConditionState.ACTIVE; // active and not acked

			r_conditionList.Add(condition1);
			r_conditionList.Add(condition2);

		}// end AtSecond10

		public void AtSecond30(ref ArrayList r_conditionList)
		{
			condition1.Severity = 540;
			condition1.Message = "between [30,50]";
			condition1.AckRequired = true;
			condition1.ActiveSubConditionName = "[30,50]";
			condition1.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.SUBCONDITION |
				EnumConditionChange.ATTRIBUTE);
			condition1.StateChange = EnumConditionState.ACTIVE; // active and not acked
			r_conditionList.Add(condition1);
		} // end AtSecond30

		public void AtSecond50(ref ArrayList r_conditionList)
		{

			condition1.Severity = 1;
			condition1.Message = "not between [10,50]";
			condition1.AckRequired = false;
			condition1.ActiveSubConditionName = String.Empty;
			condition1.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.SUBCONDITION |
				EnumConditionChange.ATTRIBUTE);
			condition1.StateChange = EnumConditionState.NO; // not active

			condition2.Severity = 1;
			condition2.Message = "not between [10,50]";
			condition2.AckRequired = false;
			condition2.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.ATTRIBUTE);

			condition2.StateChange = EnumConditionState.NO; // not active
			r_conditionList.Add(condition1);
			r_conditionList.Add(condition2);

		} // end AtSecond50

		public void BetweenSecond10and30(ref ArrayList r_conditionList)
		{
			condition1.Severity = 540;
			condition1.Message = "between [30,50]";
			condition1.AckRequired = true;
			condition1.ActiveSubConditionName = "[30,50]";
			condition1.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.SUBCONDITION |
				EnumConditionChange.ATTRIBUTE);
			condition1.StateChange = EnumConditionState.ACTIVE; // active and not acked

			condition2.Severity = 510;
			condition2.Message = "between [10,50]";
			condition2.AckRequired = true;
			condition2.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.ATTRIBUTE);
			condition2.StateChange = EnumConditionState.ACTIVE; // active and not acked
			r_conditionList.Add(condition1);
			r_conditionList.Add(condition2);
		}// end BetweenSecond10and30

		public void BetweenSecond30and50(ref ArrayList r_conditionList)
		{
			condition1.Severity = 540;
			condition1.Message = "between [30,50]";
			condition1.AckRequired = true;
			condition1.ActiveSubConditionName = "[30,50]";
			condition1.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.SUBCONDITION |
				EnumConditionChange.ATTRIBUTE);
			condition1.StateChange = EnumConditionState.ACTIVE; // active and not acked

			condition2.Severity = 510;
			condition2.Message = "between [10,50]";
			condition2.AckRequired = true;
			condition2.ChangeMask = (ushort)(EnumConditionChange.ACTIVE_STATE |
				EnumConditionChange.SEVERITY |
				EnumConditionChange.MESSAGE |
				EnumConditionChange.ATTRIBUTE);
			condition2.StateChange = EnumConditionState.ACTIVE; // active and not acked										

			r_conditionList.Add(condition1);
			r_conditionList.Add(condition2);
		}// end BetweenSecond30and50

		public void AddSimpleEventSecond(ref ArrayList r_events, ArrayList attributeValues)
		{
			AeEvent secondEvent = new AeEvent(EnumEventType.SIMPLE, "computer/clock/timer", CAT_ID_TIME_TICK);
			secondEvent.Severity = 10;
			secondEvent.ActorId = "sample";
			secondEvent.Message = "Second Changed";
			secondEvent.AttributeValueList = attributeValues;
			r_events.Add(secondEvent);
		}// end AddSimpleEventSecond

		public void AddSimpleEventMinute(ref ArrayList r_events, ArrayList attributeValues)
		{
			AeEvent minuteEvent = new AeEvent(EnumEventType.SIMPLE, "computer/clock/timer", CAT_ID_TIME_TICK);
			minuteEvent.Severity = 490;
			minuteEvent.ActorId = "sample";
			minuteEvent.Message = "Minute Changed";
			minuteEvent.AttributeValueList = attributeValues;
			r_events.Add(minuteEvent);
		}// end AddSimpleEventMinute

		public void AddSimpleEventHour(ref ArrayList r_events, ArrayList attributeValues)
		{
			AeEvent hourEvent = new AeEvent(EnumEventType.SIMPLE, "computer/clock/timer", CAT_ID_TIME_TICK);
			hourEvent.Severity = 990;
			hourEvent.ActorId = "sample";
			hourEvent.Message = "Hour Changed";
			hourEvent.AttributeValueList = attributeValues;
			r_events.Add(hourEvent);
		}// end AddSimpleEventHour

		//--
		#endregion

	}   //	end OpcServer
}   //	end namespace
