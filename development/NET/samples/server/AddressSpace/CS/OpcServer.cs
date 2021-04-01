using System;
using System.Collections;
using System.Threading;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace AddressSpace
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

		#region Public Attributes
		//-----------------------

		static public AutoResetEvent AddressSpaceChanged = null;
		public MyDaAddressSpaceElement[] ReconfigHandles = null;
		public MyDaAddressSpaceElement ObjectBased = null;

		//-
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
		}   //	end ServiceName

		public string ServiceDescription
		{
			get
			{
				return Application.Instance.ServiceDescription;
			}
			set
			{
				Application.Instance.ServiceDescription = value;
			}
		}   //	end ServiceDescription

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
				Application.Instance.ClsidDa = "{BCD4EC73-1C00-4F28-AA15-8BC5F8023E36}";
				Application.Instance.ProgIdDa = "Softing.OPCToolbox.NET.CS.Sample.AddressSpace.DA.1";
				Application.Instance.VerIndProgIdDa = "Softing.OPCToolbox.NET.CS.Sample.AddressSpace.DA";
				Application.Instance.Description = "Softing OPC Toolkit .NET CS AddressSpace Sample Server";
				Application.Instance.IpPortHTTP = 8079;
				Application.Instance.UrlDa = "/OPC/DA";
				Application.Instance.MajorVersion = 4;
				Application.Instance.MinorVersion = 47;
				Application.Instance.PatchVersion = 1;
				Application.Instance.BuildNumber = 0;
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH";
				Application.Instance.MinUpdateRateDa = 100;
				Application.Instance.ServiceName = "";
				Application.Instance.ClientCheckPeriod = 30000;
				Application.Instance.AddressSpaceDelimiter = '.';
				Application.Instance.PropertyDelimiter = '#';

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
			}   //	end if

			return result;
		}   //	end Prepare

		public int ProcessCommandLine(string commandLine)
		{
			return Application.Instance.ProcessCommandLine(commandLine);
		}   //	end ProcessCommandLine

		public void BuildAddressSpace()
		{
			Random rand = new Random();
			MyDaAddressSpaceElement tag;

			ObjectBased = new MyDaAddressSpaceElement();
			ObjectBased.Name = "ObjectBased";
			ObjectBased.IoMode = EnumIoMode.NONE;
			ObjectBased.HasChildren = true;
			ObjectBased.ObjectType = 1;

			Application.Instance.DaAddressSpaceRoot.AddChild(ObjectBased);

			tag = new MyDaAddressSpaceElement();
			tag.Name = "T1";
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Byte);
			tag.IoMode = EnumIoMode.POLL;
			tag.ObjectType = 2;
			ObjectBased.AddChild(tag);

			tag = new MyDaAddressSpaceElement();
			tag.Name = "T2";
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Byte);
			tag.IoMode = EnumIoMode.POLL;
			tag.ObjectType = 2;
			ObjectBased.AddChild(tag);

			tag = new MyDaAddressSpaceElement();
			tag.Name = "T3";
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Byte);
			tag.IoMode = EnumIoMode.POLL;
			tag.ObjectType = 2;
			ObjectBased.AddChild(tag);

			ReconfigHandles[0] = new MyDaAddressSpaceElement();
			ReconfigHandles[0].Name = String.Format("R{0}", rand.Next());
			ReconfigHandles[0].AccessRights = EnumAccessRights.READWRITEABLE;
			ReconfigHandles[0].Datatype = typeof(Int32);
			ReconfigHandles[0].IoMode = EnumIoMode.REPORT;
			ReconfigHandles[0].HasChildren = true;
			ReconfigHandles[0].ObjectType = 3;
			ObjectBased.AddChild(ReconfigHandles[0]);

			ReconfigHandles[1] = new MyDaAddressSpaceElement();
			ReconfigHandles[1].Name = String.Format("R{0}", rand.Next());
			ReconfigHandles[1].AccessRights = EnumAccessRights.READWRITEABLE;
			ReconfigHandles[1].Datatype = typeof(Int32);
			ReconfigHandles[1].IoMode = EnumIoMode.REPORT;
			ReconfigHandles[1].ObjectType = 4;
			ObjectBased.AddChild(ReconfigHandles[1]);

			ReconfigHandles[2] = new MyDaAddressSpaceElement();
			ReconfigHandles[2].Name = String.Format("R{0}", rand.Next());
			ReconfigHandles[2].AccessRights = EnumAccessRights.READWRITEABLE;
			ReconfigHandles[2].Datatype = typeof(Int32);
			ReconfigHandles[2].IoMode = EnumIoMode.REPORT;
			ReconfigHandles[2].ObjectType = 5;
			ReconfigHandles[0].AddChild(ReconfigHandles[2]);

		}

		public void RebuildRandomAddressSpace(AddressSpaceElement objBase)
		{
			if (objBase == null)
				return;
			if (!objBase.Name.Equals("ObjectBased"))
				return;

			Random rand = new Random();

			ReconfigHandles[0] = new MyDaAddressSpaceElement();
			ReconfigHandles[0].Name = String.Format("R{0}", rand.Next());
			ReconfigHandles[0].AccessRights = EnumAccessRights.READWRITEABLE;
			ReconfigHandles[0].Datatype = typeof(Int32);
			ReconfigHandles[0].IoMode = EnumIoMode.REPORT;
			ReconfigHandles[0].HasChildren = true;
			ReconfigHandles[0].ObjectType = 3;
			objBase.AddChild(ReconfigHandles[0]);

			ReconfigHandles[1] = new MyDaAddressSpaceElement();
			ReconfigHandles[1].Name = String.Format("R{0}", rand.Next());
			ReconfigHandles[1].AccessRights = EnumAccessRights.READWRITEABLE;
			ReconfigHandles[1].Datatype = typeof(Int32);
			ReconfigHandles[1].IoMode = EnumIoMode.REPORT;
			ReconfigHandles[1].ObjectType = 4;
			objBase.AddChild(ReconfigHandles[1]);

			ReconfigHandles[2] = new MyDaAddressSpaceElement();
			ReconfigHandles[2].Name = String.Format("R{0}", rand.Next());
			ReconfigHandles[2].AccessRights = EnumAccessRights.READWRITEABLE;
			ReconfigHandles[2].Datatype = typeof(Int32);
			ReconfigHandles[2].IoMode = EnumIoMode.REPORT;
			ReconfigHandles[2].ObjectType = 5;
			ReconfigHandles[0].AddChild(ReconfigHandles[2]);
		}

		public void ShowObjectTree()
		{

			System.Console.WriteLine("CURRENT ADDRESS SPACE");
			IAddressSpaceElement root = Application.Instance.DaAddressSpaceRoot as IAddressSpaceElement;
			int level = 0;

			PrintChildren(root, ref level);

			System.Console.WriteLine(String.Format("Press Ctrl-C to exit"));
		}

		public void PrintChildren(IAddressSpaceElement element, ref int bLevel)
		{

			ArrayList children = element.GetChildren();

			foreach (DaAddressSpaceElement child in children)
			{
				if (child == null)
				{
					continue;
				}

				string line = string.Empty;
				for (byte i = 0; i < bLevel; i++)
				{
					line += string.Format("  ");
				}   //	end for

				System.Console.WriteLine(string.Format("{0}[{1}]", line, child.Name));

				if (child.HasChildren == true)
				{
					bLevel++;
					PrintChildren(child, ref bLevel);
					bLevel--;
				}   //	end if

			}   //	end foreach

		} //	end PrintChildren

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
