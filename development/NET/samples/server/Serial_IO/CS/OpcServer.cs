using System;
using System.Collections;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace SerialIO
{
	public class OpcServer
	{

		#region Constructor
		//-----------------
		public OpcServer()
		{

		}   //	end constructor
			//--
		#endregion

		#region Protected Attributes
		//--------------------------

		protected ShutdownHandler m_shutdownRequest = null;
		protected MyDaAddressSpaceElement m_daSimulationElement1 = null;
		protected MyDaAddressSpaceElement m_daSimulationElement2 = null;
		protected Random m_random = new Random(1);

		protected object m_requestsJanitor = new object();
		protected ArrayList m_requests = new ArrayList();

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

				Application.Instance.ClsidDa = "{381057C3-BCEE-4DA5-A05A-66DCE07F3576}";

				Application.Instance.ProgIdDa = "Softing.OPCToolbox.NET.CS.Sample.SerialIO.DA.1";
				Application.Instance.VerIndProgIdDa = "Softing.OPCToolbox.NET.CS.Sample.SerialIO.DA";
				Application.Instance.Description = "Softing OPC Toolkit .NET C# SerialIO Sample Server";
				Application.Instance.MajorVersion = 4;
				Application.Instance.MinorVersion = 47;
				Application.Instance.PatchVersion = 1;
				Application.Instance.BuildNumber = 0;
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH";
				Application.Instance.MinUpdateRateDa = 100;
				Application.Instance.ClientCheckPeriod = 30000;
				Application.Instance.AddressSpaceDelimiter = '.';
				Application.Instance.PropertyDelimiter = '/';

				Application.Instance.IpPortHTTP = 8079;
				Application.Instance.UrlDa = "/OPC/DA";
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
			int result = Application.Instance.Initialize(aMyCreator);

			if (ResultCode.SUCCEEDED(result))
			{
				Application.Instance.EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.SERVER,
					EnumTraceGroup.SERVER,
					"Trace1.txt",
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
				MyCreator creator = (MyCreator)Application.Instance.Creator;

				//	DA
				DaAddressSpaceRoot daRoot = Application.Instance.DaAddressSpaceRoot;
				m_daSimulationElement1 = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				m_daSimulationElement1.Name = "simulation1";
				m_daSimulationElement1.AccessRights = EnumAccessRights.READWRITEABLE;
				m_daSimulationElement1.Datatype = typeof(System.Int32);
				m_daSimulationElement1.IoMode = EnumIoMode.POLL;
				daRoot.AddChild(m_daSimulationElement1);

				m_daSimulationElement2 = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				m_daSimulationElement2.Name = "simulation2";
				m_daSimulationElement2.AccessRights = EnumAccessRights.READWRITEABLE;
				m_daSimulationElement2.Datatype = typeof(System.Int32);
				m_daSimulationElement2.IoMode = EnumIoMode.POLL;
				daRoot.AddChild(m_daSimulationElement2);
			}
			catch (Exception exc)
			{
				Trace(
					EnumTraceLevel.ERR,
					EnumTraceGroup.USER1,
					"OpcServer.BuildAddressSpace",
					exc.ToString());
				return (int)EnumResultCode.E_FAIL;
			}   //	end try...catch

			return (int)EnumResultCode.S_OK;
		}   //	end BuildAddressSpace


		public void ChangeSimulationValues()
		{
			if (m_daSimulationElement1 != null)
			{
				m_daSimulationElement1.ValueChanged(new ValueQT(m_random.Next(5000), EnumQuality.GOOD, DateTime.Now));
			}   //	end if

			if (m_daSimulationElement2 != null)
			{
				m_daSimulationElement2.ValueChanged(new ValueQT(m_random.Next(5000), EnumQuality.GOOD, DateTime.Now));
			}   //	end if

		}   //	end ChangeSimulationValues

		public void AddRequsts(ArrayList requests)
		{
			foreach (MyRequest request in requests)
			{
				//	set the requests on pending state. 
				//	they will automatically be set to complete state when complete method is called
				request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.PENDING);
			}
			lock (m_requestsJanitor)
			{
				m_requests.AddRange(requests);
			}
		}


		public void HandleRequests()
		{
			lock (m_requestsJanitor)
			{
				foreach (MyRequest request in m_requests)
				{
					if (request.ProgressRequestState(EnumRequestState.PENDING, EnumRequestState.INPROGRESS))
					{
						MyDaAddressSpaceElement element = request.AddressSpaceElement as MyDaAddressSpaceElement;
						if (element == null)
						{
							request.Result = EnumResultCode.E_FAIL;
						}
						else
						{
							switch (request.TransactionType)
							{
								case EnumTransactionType.READ:
									if (request.PropertyId == 0)
									{
										ValueQT cacheValue = null;
										request.Result = (EnumResultCode)element.GetCacheValue(ref cacheValue);
										request.Value = cacheValue;
									}
									else
									{
										element.GetPropertyValue(request);
									}   //  end if ... else
									break;

								case EnumTransactionType.WRITE:
									if (request.PropertyId == 0)
									{
										ValueQT newValue = request.Value;
										request.Result = (EnumResultCode)element.ValueChanged(newValue);
									}
									else
									{
										request.Result = EnumResultCode.E_BADRIGHTS;
									}   //  end if ... else
									break;

								default:
									break;
							}   //  end switch

						}   //  end if ... else
					}   //  end if ... else
					request.Complete();
				}   //  end forech
				m_requests.Clear();
			}   //  end lock

		}   //  end HandleRequests


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

	}   //	end OpcServer
}   //	end namespace
