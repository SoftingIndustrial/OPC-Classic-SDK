using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace DllSample
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
			
		}	//	end constructor
		//--
		#endregion

		#region Protected Attributes
		//--------------------------

		protected ShutdownHandler m_shutdownRequest = null;
		protected MyDaAddressSpaceElement m_daSimulationElement = null;
		protected Random m_random = new Random(1);
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

		#region Public Methods
		//-----------------------

		public static int Shutdown()
		{
			return (int)EnumResultCode.S_OK;
		}	//	end Shutdown

		public int Initialize()
		{
			try
			{
				Application.Instance.VersionOtb = 447;
				Application.Instance.AppType = EnumApplicationType.EXECUTABLE;

				Application.Instance.ClsidDa = "{6EB07DA1-88D7-49EA-A9D7-15B57A80C61F}";
				Application.Instance.ProgIdDa = "Softing.OPCToolbox.NET.CS.Sample.Dll.DA.1";
				Application.Instance.VerIndProgIdDa = "Softing.OPCToolbox.NET.CS.Sample.Dll.DA";
				
				Application.Instance.ClsidAe = "{E22FA50D-E5B9-474B-B326-3B1BEC7B958F}";
				Application.Instance.ProgIdAe = "Softing.OPCToolbox.NET.CS.Sample.Dll.AE.1";
				Application.Instance.VerIndProgIdAe = "Softing.OPCToolbox.NET.CS.Sample.Dll.AE";

				Application.Instance.Description = "Softing OPC Toolkit .NET C# Dll Sample Server";
				
				Application.Instance.MajorVersion = 4;
				Application.Instance.MinorVersion = 47;
				Application.Instance.PatchVersion = 1;
				Application.Instance.BuildNumber = 0;
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH";
				Application.Instance.MinUpdateRateDa = 100;
				Application.Instance.ClientCheckPeriod = 30000;
				Application.Instance.AddressSpaceDelimiter = '.';
				Application.Instance.PropertyDelimiter = '/';
				Application.Instance.SupportDisableConditions = true;
				
				Application.Instance.IpPortHTTP = 8079;
				Application.Instance.UrlDa = "/OPC/DA";
				Application.Instance.ShutdownRequest += new Softing.OPCToolbox.Server.ShutdownHandler(Shutdown);
			}
			catch(Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer::Initialize",
					exc.ToString());

				return (int)EnumResultCode.E_FAIL;
			}	//	end try...catch
			return (int)EnumResultCode.S_OK;
		}	//	end Initialize
		
		public int Start()
		{
			return Application.Instance.Start();
		}	//	end Start

		public int Stop()
		{
			return Application.Instance.Stop();
		}	//	end Stop

		public int Ready()
		{
			return Application.Instance.Ready();
		}	//	end Ready

		public int Terminate()
		{
			return Application.Instance.Terminate();
		}	//	end Terminate

		public int Prepare(MyCreator aMyCreator)
		{
			int result = (int)EnumResultCode.S_OK;

			//	TODO - binary license activation
			//	Fill in your binary license activation keys here
			//
			//	NOTE: you can activate one or all of the features at the same time
			//	firstly activate the COM-DA Server feature
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
			}	//	end if

			return result;
		}	//	end Prepare

		public int ProcessCommandLine(string commandLine)
		{
			return Application.Instance.ProcessCommandLine(commandLine);
		}	//	end ProcessCommandLine
	
		public int BuildAddressSpace()
		{
			try
			{
				MyCreator creator = (MyCreator)Application.Instance.Creator;

				//	DA				
				DaAddressSpaceRoot daRoot = Application.Instance.DaAddressSpaceRoot;
				m_daSimulationElement = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				m_daSimulationElement.Name = "simulation";
				m_daSimulationElement.AccessRights = EnumAccessRights.READWRITEABLE;
				m_daSimulationElement.Datatype = typeof(System.Int32);				
				m_daSimulationElement.IoMode = EnumIoMode.POLL;				
				daRoot.AddChild(m_daSimulationElement);

				m_daSimulationElement.ValueChanged(
					new ValueQT(
						m_random.Next(5000),
						EnumQuality.GOOD,
						DateTime.Now));				
				//AE
				AeAddressSpaceRoot aeRoot = Application.Instance.AeAddressSpaceRoot;
				AeAddressSpaceElement aeElement = creator.CreateAeAddressSpaceElement();
				aeElement.Name = "SimulationElement";
				aeElement.HasChildren = false;
				aeRoot.AddChild(aeElement);
			}
			catch(Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer:BuildAddressSpace",
					exc.ToString());
				return (int)EnumResultCode.E_FAIL;
			}	//	end try...catch

			return (int)EnumResultCode.S_OK;
		}	//	end BuildAddressSpace
		public int BuildEventCategories()
		{
			try
			{
				AeCategory simulationCategory = new AeCategory(
												EnumEventType.SIMPLE,
												CATEGORY_ID_SIMULATION,
												"SimulationCategory");

				Application.Instance.AddAeCategory(simulationCategory);
				simulationCategory.AddAttribute(
					ATTRIBUTE_ID_SIMULATION,
					"SimulationAttribute",
					typeof(System.UInt16));
			}
			catch(Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer::BuildEventCategories",
					exc.ToString());
				return (int)EnumResultCode.E_FAIL;
			}	//	end try...catch

			return (int)EnumResultCode.S_OK;

		}	//	end BuildEventCategories

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
		}	//	end FireEvents
		public void ChangeSimulationValues()
		{
			if (m_daSimulationElement != null)
			{
				m_daSimulationElement.ValueChanged(
					new ValueQT(
						m_random.Next(5000),
						EnumQuality.GOOD,
						DateTime.Now));
						
			}	//	end if
		}	//	end ChangeSimulationValues
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

	}	//	end OpcServer
}	//	end namespace
