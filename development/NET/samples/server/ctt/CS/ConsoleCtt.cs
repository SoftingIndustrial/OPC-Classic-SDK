using System;
using System.Threading;
using System.Collections;
using System.Runtime.InteropServices;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace ConsoleCtt
{

	class MyTransaction : DaTransaction
	{

		public MyTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[] aRequestList,
			uint aSessionHandle) :
			base(aTransactionType, aRequestList, aSessionHandle)
		{

		}   //	enc ctr

		public override int HandleReadRequests()
		{

			lock (m_requestJanitor)
			{

				int count = Requests.Count;

				for (int i = 0; i < count; i++)
				{

					DaRequest currentRequest = Requests[i] as DaRequest;

					if ((currentRequest != null) && (currentRequest.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) == true))
					{

						MyDaAddressSpaceElement requestElement = currentRequest.AddressSpaceElement as MyDaAddressSpaceElement;

						if (requestElement != null)
						{

							if (currentRequest.PropertyId == 0)
							{

								ValueQT cacheValue = null;
								requestElement.GetCacheValue(ref cacheValue);
								currentRequest.Value = cacheValue;
								currentRequest.Result = EnumResultCode.S_OK;
							}
							else
							{
								requestElement.GetPropertyValue(currentRequest);
							}   //	end if ... else
						}
						else
						{
							currentRequest.Result = EnumResultCode.E_FAIL;
						}   //	end if ... else
					}
					else
					{
						// this is not a request ???
						Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.USER1,
							"MyTransaction", "HandleReadRequests - received a non request element in the request's array List");
					}   //	end if ... else

				}   //	end for

				return CompleteRequests();
			}
		}   //	end HandleReadRequests

		public override int HandleWriteRequests()
		{

			lock (m_requestJanitor)
			{

				int count = Requests.Count;
				for (int i = 0; i < count; i++)
				{

					DaRequest currentRequest = Requests[i] as DaRequest;
					if ((currentRequest != null) && (currentRequest.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) == true))
					{
						MyDaAddressSpaceElement requestElement = currentRequest.AddressSpaceElement as MyDaAddressSpaceElement;
						if (requestElement != null)
						{
							currentRequest.Result = (EnumResultCode)requestElement.ValueChanged(currentRequest.Value);
						}   //	end if
					}   //	end if
				}   //	end for

				return CompleteRequests();
			}   //  end lock

		}   //	end HandleWriteRequests

		private static object m_requestJanitor = new object();

	}   //	end class MyTransaction


	class MyCreator : Creator
	{

		public override DaTransaction CreateTransaction(
			EnumTransactionType aTransactionType,
			DaRequest[] aRequests,
			uint aSessionHandle)
		{

			return (DaTransaction)new MyTransaction(aTransactionType, aRequests, aSessionHandle);
		}   //	end CreateTransaction

		public override DaAddressSpaceElement CreateDaAddressSpaceElement()
		{
			return (DaAddressSpaceElement)new MyDaAddressSpaceElement();
		}   //	end CreateDaAddressSpaceElement

	}   //	end class MyCreator


	class MyDaAddressSpaceElement : DaAddressSpaceElement
	{

		static public byte TYP_UNKNOWN = 0x0;
		static public byte TYP_NODE = 0x80;
		static public byte TYP_TAG = 0x08;

		private byte m_Type = TYP_UNKNOWN;

		public virtual byte Type
		{
			get { return m_Type; }
			set { m_Type = value; }
		}   //	end Type

		public MyDaAddressSpaceElement()
		{

		}   //	end ctr

		public override int QueryProperties(out ArrayList aPropertyList)
		{

			try
			{

				DaProperty property = new DaProperty();
				property.Id = 101;
				property.AccessRights = EnumAccessRights.READABLE;
				property.Datatype = typeof(string);
				property.Name = "Descripiton";
				property.ItemId = "Descripiton";
				property.Description = "Item Descripiton";

				aPropertyList = new ArrayList();
				((ArrayList)aPropertyList).Add(property);

				return (int)(EnumResultCode.S_OK);
			}
			catch
			{
				aPropertyList = null;
				return (int)(EnumResultCode.E_FAIL);
			}   //	end catch

		}   //	end QueryProperties

		/// <summary>
		/// Get elements property value data
		/// </summary>
		public void GetPropertyValue(DaRequest aRequest)
		{

			if (aRequest.PropertyId == 101)
			{
				aRequest.Value = new ValueQT("item description", EnumQuality.GOOD, DateTime.Now);
				aRequest.Result = EnumResultCode.S_OK;
			}   //	end if

		}   //	end GetPropertyValue

	}   //	end class MyDaAddressSpaceElement


	class ConsoleCttCSSample
	{

		static public AutoResetEvent m_endEvent;    // signal application should terminate

		static void initServer()
		{

			Application.Instance.VersionOtb = 447;
			Application.Instance.AppType = EnumApplicationType.EXECUTABLE;
			Application.Instance.ClsidDa = "{D3B19EEC-7752-46b9-BB6D-63BD70D2CF60}";
			Application.Instance.ProgIdDa = "Softing.OPCToolbox.NET.CS.Smpl.CTT.DA";
			Application.Instance.VerIndProgIdDa = "Softing.OPCToolbox.NET.CS.Smpl.CTT.DA.1";

			Application.Instance.Description = "Softing OPC Toolkit.NET C# CTT Server Sample";
			Application.Instance.ServiceName = "";
			Application.Instance.IpPortHTTP = 8079;
			Application.Instance.UrlDa = "/OPC/DA";
			Application.Instance.MajorVersion = 4;
			Application.Instance.MinorVersion = 47;
			Application.Instance.PatchVersion = 1;
			Application.Instance.BuildNumber = 0;
			Application.Instance.VendorInfo = "Softing Industrial Automation GmbH";
			Application.Instance.MinUpdateRateDa = 100;
			Application.Instance.ClientCheckPeriod = 30000;
			Application.Instance.AddressSpaceDelimiter = '.';
			Application.Instance.PropertyDelimiter = '#';
			Application.Instance.ShutdownRequest += new Softing.OPCToolbox.Server.ShutdownHandler(HandleShutdown);

		}   //	end initServer


		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main(string[] args)
		{
			int ret = (int)EnumResultCode.S_OK;

			m_endEvent = new AutoResetEvent(false);

			MyWin32.HandlerRoutine hr = new MyWin32.HandlerRoutine(MyWin32.Handler);
			MyWin32.SetConsoleCtrlHandler(hr, true);

			initServer();

			//	TODO - binary license activation
			//	Fill in your binary license activation keys here
			//
			//	NOTE: you can activate one or all of the features at the same time
			//	firstly activate the COM-DA Server feature
			ret = Application.Instance.Activate(EnumFeature.DA_SERVER, "0d20-004c-c458-991e-3d70");
			if (!ResultCode.SUCCEEDED(ret))
			{
				return;
			}

			//	activate the XML-DA Server Feature
			ret = Application.Instance.Activate(EnumFeature.XMLDA_SERVER, "0d40-004c-7638-9373-80b4");
			if (!ResultCode.SUCCEEDED(ret))
			{
				return;
			}

			//	activate the COM-AE Server Feature
			ret = Application.Instance.Activate(EnumFeature.AE_SERVER, "0d30-004c-9d48-035d-7717");
			if (!ResultCode.SUCCEEDED(ret))
			{
				return;
			}

			//	END TODO - binary license activation

			if (ResultCode.SUCCEEDED(Application.Instance.Initialize(new MyCreator())))
			{

				Application.Instance.EnableTracing(
					EnumTraceGroup.ALL,
					EnumTraceGroup.ALL,
					EnumTraceGroup.SERVER,
					EnumTraceGroup.SERVER,
					"Trace.txt",
					1000000,
					2);

				if ((ret = Application.Instance.ProcessCommandLine(Environment.CommandLine)) == (int)EnumResultCode.S_OK)
				{

					if (ResultCode.SUCCEEDED(Application.Instance.Start()))
					{

						BuildNamespace();
						Application.Instance.Ready();
						Console.WriteLine("Press Ctrl-C to exit");

						bool bEnd = false;
						DateTime dt = DateTime.UtcNow;

						Random rand = new Random();
						while (!bEnd)
						{

							// wait for keyboard entry 'Ctrl + C' or 'Ctrl + Break'
							if ((bEnd = m_endEvent.WaitOne(1000, false)) == true)
							{
								continue;
							}   //	end if
						}   //	end while
						Application.Instance.Stop();
					}   //	end if
				}
				else
				{

					if (ret == (uint)EnumResultCode.S_FALSE)
					{
						System.Console.WriteLine("Registration operation {0}", Environment.CommandLine);
					}
				}
				Application.Instance.Terminate();
			}
		}

		public static void BuildNamespace()
		{
			MyDaAddressSpaceElement tag = null, node = null;
			ValueQT val = null;


			node = new MyDaAddressSpaceElement();
			node.Name = "CTT";
			node.Type = MyDaAddressSpaceElement.TYP_NODE;
			node.IoMode = EnumIoMode.NONE;
			node.HasChildren = true;
			Application.Instance.DaAddressSpaceRoot.AddChild(node);

			tag = new MyDaAddressSpaceElement();
			tag.Name = "I1";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(SByte);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((sbyte)1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}   //	end if

			tag = new MyDaAddressSpaceElement();
			tag.Name = "UI1";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Byte);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((byte)1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "I2";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Int16);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((Int16)1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "UI2";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(UInt16);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((UInt16)1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "I4";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Int32);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((Int32)1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "UI4";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(UInt32);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((UInt32)1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "I8";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Int64);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((Int64)1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "UI8";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(UInt64);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((UInt64)1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "R4";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Single);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((Single)1.0, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "R8";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Double);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT((Double)1.0, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "BOOL";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Boolean);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT(true, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "BSTR";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(String);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT("CTT", EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "DATE";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(DateTime);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				val = new ValueQT(DateTime.Now, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "I1 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(SByte[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				SByte[] v = new SByte[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "UI1 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Byte[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				Byte[] v1 = new Byte[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v1, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "I2 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Int16[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				Int16[] v2 = new Int16[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v2, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "UI2 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(UInt16[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				UInt16[] v3 = new UInt16[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v3, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

			tag = new MyDaAddressSpaceElement();
			tag.Name = "I4 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Int32[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				Int32[] v4 = new Int32[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v4, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

			tag = new MyDaAddressSpaceElement();
			tag.Name = "UI4 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(UInt32[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				UInt32[] v5 = new UInt32[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v5, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

			tag = new MyDaAddressSpaceElement();
			tag.Name = "I8 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Int64[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				Int64[] v6 = new Int64[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v6, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

			tag = new MyDaAddressSpaceElement();
			tag.Name = "UI8 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(UInt64[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				UInt64[] v7 = new UInt64[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v7, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

			tag = new MyDaAddressSpaceElement();
			tag.Name = "R4 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Single[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				Single[] v8 = new Single[5] { 0, 1, 2, 3, 4 };
				val = new ValueQT(v8, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}
			tag = new MyDaAddressSpaceElement();
			tag.Name = "R8 array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Double[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				Double[] v9 = new Double[5] { 0.0, 1.0, 2.0, 3.0, 4.0 };
				val = new ValueQT(v9, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

			tag = new MyDaAddressSpaceElement();
			tag.Name = "BOOL array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(Boolean[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				bool[] va = new bool[5] { true, false, true, false, true };
				val = new ValueQT(va, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

			tag = new MyDaAddressSpaceElement();
			tag.Name = "BSTR array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(String[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				string[] vb = new string[5] { "A", "B", "C", "D", "E" };
				val = new ValueQT(vb, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

			tag = new MyDaAddressSpaceElement();
			tag.Name = "DATE array";
			tag.Type = MyDaAddressSpaceElement.TYP_TAG;
			tag.AccessRights = EnumAccessRights.READWRITEABLE;
			tag.Datatype = typeof(DateTime[]);
			tag.IoMode = EnumIoMode.POLL;
			if (node.AddChild(tag))
			{
				DateTime[] vc = new DateTime[5];
				vc[0] = DateTime.Now; vc[1] = DateTime.Now; vc[2] = DateTime.Now; vc[3] = DateTime.Now; vc[4] = DateTime.Now;
				val = new ValueQT(vc, EnumQuality.GOOD, DateTime.Now);
				tag.ValueChanged(val);
			}

		}   //	end BuildNamespace

		public static int HandleShutdown()
		{

			m_endEvent.Set();
			return (int)EnumResultCode.S_OK;
		}   //	end HandleShutdown
	}


	#region CONSOLE_CONTROL_HANDLING

	// A simple class that exposes two static Win32 functions.
	// One is a delegate type and the other is an enumerated type.
	public class MyWin32
	{
		// Declare the SetConsoleCtrlHandler function 
		// as external and receiving a delegate.   
		[DllImport("Kernel32")]
		public static extern Boolean SetConsoleCtrlHandler(HandlerRoutine Handler,
			Boolean Add);

		// A delegate type to be used as the handler routine 
		// for SetConsoleCtrlHandler.
		public delegate Boolean HandlerRoutine(CtrlTypes CtrlType);

		// An enumerated type for the control messages 
		// sent to the handler routine.
		public enum CtrlTypes
		{
			CTRL_C_EVENT = 0,
			CTRL_BREAK_EVENT,
			CTRL_CLOSE_EVENT,
			CTRL_LOGOFF_EVENT = 5,
			CTRL_SHUTDOWN_EVENT
		}

		// A private static handler function in the MyApp class.
		static public Boolean Handler(MyWin32.CtrlTypes CtrlType)
		{
			String message = "This message should never be seen!";

			// A switch to handle the event type.
			switch (CtrlType)
			{
				case MyWin32.CtrlTypes.CTRL_C_EVENT:
				case MyWin32.CtrlTypes.CTRL_BREAK_EVENT:
					message = "stop execution, since CTRL command!";
					ConsoleCttCSSample.m_endEvent.Set();
					break;
				case MyWin32.CtrlTypes.CTRL_CLOSE_EVENT:
					message = "A CTRL_CLOSE_EVENT was raised by the user.";
					break;
				case MyWin32.CtrlTypes.CTRL_LOGOFF_EVENT:
					message = "A CTRL_LOGOFF_EVENT was raised by the user.";
					break;
				case MyWin32.CtrlTypes.CTRL_SHUTDOWN_EVENT:
					message = "A CTRL_SHUTDOWN_EVENT was raised by the user.";
					break;
			}

			// Use interop to display a message for the type of event.
			Console.WriteLine(message);

			return true;
		}
	}
	#endregion
}
