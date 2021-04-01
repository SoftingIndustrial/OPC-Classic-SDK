using System;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace UpdateMode
{
	public enum EnumDaVariableType
	{
		SEC = 0,
		MIN
	}   //	end enum

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
		MyDaAddressSpaceElement m_reportTagSec = null;
		MyDaAddressSpaceElement m_reportTagMin = null;

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

				Application.Instance.ClsidDa = "{B868BF70-6F81-4286-8AD3-44F4E4E2E192}";
				Application.Instance.ProgIdDa = "Softing.OPCToolbox.NET.CS.Sample.UpdateMode.DA.1";
				Application.Instance.VerIndProgIdDa = "Softing.OPCToolbox.NET.CS.Sample.UpdateMode.DA";
				Application.Instance.Description = "Softing OPC Toolkit .NET C# UpdateMode Sample Server";
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
			int result = (int)EnumResultCode.S_OK;
			//	TODO - binary license activation
			//	Fill in your binary license activation keys here
			//
			//	NOTE: you can activate one or all of the features at the same time
			//	firstly activate the COM-DA Server feature

			//result = Application.Instance.Activate(EnumFeature.DA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
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
				DaAddressSpaceRoot root = Application.Instance.DaAddressSpaceRoot;
				MyCreator creator = (MyCreator)Application.Instance.Creator;

				MyDaAddressSpaceElement clockNode = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				clockNode.Name = "clock";
				clockNode.Type = MyDaAddressSpaceElement.TYPE_NODECLOCK;
				clockNode.IoMode = EnumIoMode.NONE;
				clockNode.HasChildren = true;
				root.AddChild(clockNode);

				MyDaAddressSpaceElement reportNode = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				reportNode.Name = "report";
				reportNode.Type = MyDaAddressSpaceElement.TYPE_NODEREPORT;
				reportNode.IoMode = EnumIoMode.NONE;
				reportNode.HasChildren = true;
				clockNode.AddChild(reportNode);

				m_reportTagSec = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				m_reportTagSec.Name = "sec";
				m_reportTagSec.Type = MyDaAddressSpaceElement.TYPE_REPORT_SEC;
				m_reportTagSec.AccessRights = EnumAccessRights.READABLE;
				m_reportTagSec.Datatype = typeof(Int32);
				m_reportTagSec.IoMode = EnumIoMode.REPORT_CYCLIC;
				reportNode.AddChild(m_reportTagSec);

				m_reportTagMin = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				m_reportTagMin.Name = "min";
				m_reportTagMin.Type = MyDaAddressSpaceElement.TYPE_REPORT_MIN;
				m_reportTagMin.AccessRights = EnumAccessRights.READABLE;
				m_reportTagMin.Datatype = typeof(Int32);
				m_reportTagMin.IoMode = EnumIoMode.REPORT_CYCLIC;
				reportNode.AddChild(m_reportTagMin);

				MyDaAddressSpaceElement pollNode = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				pollNode.Name = "poll";
				pollNode.Type = MyDaAddressSpaceElement.TYPE_NODEPOLL;
				pollNode.IoMode = EnumIoMode.NONE;
				pollNode.HasChildren = true;
				clockNode.AddChild(pollNode);

				MyDaAddressSpaceElement tag = null;

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				tag.Name = "sec";
				tag.Type = MyDaAddressSpaceElement.TYPE_POLL_SEC;
				tag.AccessRights = EnumAccessRights.READABLE;
				tag.Datatype = typeof(Int32);
				tag.IoMode = EnumIoMode.POLL;
				pollNode.AddChild(tag);

				tag = (MyDaAddressSpaceElement)creator.CreateMyDaAddressSpaceElement();
				tag.Name = "min";
				tag.Type = MyDaAddressSpaceElement.TYPE_POLL_MIN;
				tag.AccessRights = EnumAccessRights.READABLE;
				tag.Datatype = typeof(Int32);
				tag.IoMode = EnumIoMode.POLL;
				pollNode.AddChild(tag);

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
		public void ChangeValue(object varValue, EnumDaVariableType variableType)
		{
			ValueQT val = new ValueQT(varValue, EnumQuality.GOOD, DateTime.Now);

			switch (variableType)
			{
				case EnumDaVariableType.SEC:
					if (m_reportTagSec != null)
					{
						m_reportTagSec.ValueChanged(val);
					}
					break;

				case EnumDaVariableType.MIN:
					if (m_reportTagMin != null)
					{
						m_reportTagMin.ValueChanged(val);
					}
					break;

				default:
					break;
			}

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


		//--
		#endregion

	}   //	end OpcServer
}   //	end namespace
