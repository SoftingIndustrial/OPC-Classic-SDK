using System;
using System.Threading;
using System.Runtime.InteropServices;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace BISDemo{

	#region //	Console control handling
	// A simple class that exposes two static Win32 functions.
	// One is a delegate type and the other is an enumerated type.
	public class MyWin32 {
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
		public enum CtrlTypes {
			CTRL_C_EVENT = 0,
			CTRL_BREAK_EVENT,
			CTRL_CLOSE_EVENT,   
			CTRL_LOGOFF_EVENT = 5,
			CTRL_SHUTDOWN_EVENT
		}

		// A private static handler function in the MyApp class.
		static public Boolean Handler(MyWin32.CtrlTypes CtrlType) {
			String message = "This message should never be seen!";

			// A switch to handle the event type.
			switch(CtrlType) {
				case MyWin32.CtrlTypes.CTRL_C_EVENT:
				case MyWin32.CtrlTypes.CTRL_BREAK_EVENT:
					message = "stop execution, since CTRL command!";
					BISApplication.m_endEvent.Set();
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

	#region	//	Public Toolkit Derived classes
	//----------------------------------------

	
	public class BISTransaction : DaTransaction{
		
		#region	//	Public Constructors
		//-----------------------------

		public BISTransaction(
			EnumTransactionType aTransactionType, 
			DaRequest[] aRequests,
			uint aSessionKey) : 
			base(aTransactionType, aRequests, aSessionKey){
		}	//	enc ctr
		
		//-
		#endregion

		#region	//	Public Override Methods
		//---------------------------------

		public override int HandleReadRequests(){

            int count = Requests.Count;

            for (int i = 0; i < count; i++)
            {
                DaRequest request = Requests[i] as DaRequest;

                if ((request != null) && (request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) == true)){
					DaBaseElement element = request.AddressSpaceElement as DaBaseElement;

                    if (element != null){

						element.HandleReadRequest(request);
                    }
                    else{

						request.Result = EnumResultCode.E_FAIL;
						request.Value = null;
						request.Complete();
                    }	//	end if ... else
                }
			}
			return (int)EnumResultCode.S_OK;
		}	//	end HandleReadRequests


		public override int HandleWriteRequests(){

            int count = Requests.Count;

            for (int i = 0; i < count; i++)
            {
                DaRequest request = Requests[i] as DaRequest;

                if ((request != null) && (request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) == true)){
					DaBaseElement element = request.AddressSpaceElement as DaBaseElement;
					if (element != null) {
						element.HandleWriteRequest(request);
					}
					else {
						request.Result = EnumResultCode.E_FAIL;
						request.Complete();
					}	//	end if ... else
				}	//	end if
			}
			return (int)EnumResultCode.S_OK;
		}	//	end HandleWriteRequests
		
		//-
		#endregion

	}	//	end class BISTransaction
	
	//-
	#endregion

	#region	//	Main

	/// <summary>
	/// Summary description for ConsoleOTS.
	/// </summary>
	class BISApplication{

		static public AutoResetEvent m_endEvent;    // signal application should terminate
		static public DaBaseElement device1Node = null; 
		static public DaBaseElement device2Node = null;
		
		static void initServer(){
			
			Application.Instance.VersionOtb = 447;
			Application.Instance.AppType = EnumApplicationType.EXECUTABLE;

			Application.Instance.ClsidDa = "{AC0BFADA-A6AC-429C-BFA4-5F862203209F}";
			Application.Instance.ProgIdDa = "Softing.OPCToolbox.NET.CS.BISDemo.DA.1";
			Application.Instance.VerIndProgIdDa = "Softing.OPCToolbox.NET.CS.BISDemo.DA";

			Application.Instance.ClsidAe = "{76043A64-66EB-4bbc-919F-5631B42D6060}";
			Application.Instance.ProgIdAe = "Softing.OPCToolbox.NET.CS.BISDemo.AE.1";
			Application.Instance.VerIndProgIdAe = "Softing.OPCToolbox.NET.CS.BISDemo.AE";

			Application.Instance.Description = "Softing OPC Toolkit .NET C# BIS Demo Server";
			Application.Instance.ServiceName = string.Empty;
			Application.Instance.IpPortHTTP = 0;
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
			Application.Instance.SupportDisableConditions = false;
			Application.Instance.ShutdownRequest += new Softing.OPCToolbox.Server.ShutdownHandler(Shutdown);
			
//			Application.Instance.Activate(EnumFeature.DA_SERVER, "Insert your license key here");
//			Application.Instance.Activate(EnumFeature.AE_SERVER, "Insert your license key here");
			
		} 

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		static void Main(string[] args){
			
			try{

				int ret = (int)EnumResultCode.E_FAIL;
				int result = (int)EnumResultCode.S_OK;

				m_endEvent = new AutoResetEvent(false);

				MyWin32.HandlerRoutine hr = null;
				hr = new MyWin32.HandlerRoutine(BISDemo.MyWin32.Handler);

				MyWin32.SetConsoleCtrlHandler(hr, true);

				initServer();

				//	TODO - binary license activation
				//	Fill in your binary license activation keys here
				//
				//	NOTE: you can activate one or all of the features at the same time
				//	firstly activate the COM-DA Server feature
				//	result = Application.Instance.Activate(EnumFeature.DA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
				if (!ResultCode.SUCCEEDED(result))
				{
					return;
				}

				//	activate the XML-DA Server Feature
				//	result = Application.Instance.Activate(EnumFeature.XMLDA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX");
				if (!ResultCode.SUCCEEDED(result))
				{
					return;
				}
				//	END TODO - binary license activation

				if (ResultCode.SUCCEEDED(Application.Instance.Initialize(new BISCreator()))){

					Application.Instance.EnableTracing(
						EnumTraceGroup.ALL,
						EnumTraceGroup.ALL,
						EnumTraceGroup.SERVER,
						EnumTraceGroup.SERVER,
						"Trace.txt",
						1000000,
						0);

					ret = Application.Instance.ProcessCommandLine(Environment.CommandLine);

					if  (ret == (uint)EnumResultCode.S_OK) {

						if (ResultCode.SUCCEEDED(Application.Instance.Start())) {

							BuildEventCategories();
							BuildNamespace();
							Application.Instance.Ready();
							Console.WriteLine("Press Ctrl-C to exit");
						
							bool bEnd = false;
							int simulationIndex = 0;
							Random rand = new Random();
							while (!bEnd) {
								
								// wait for keyboard entry 'Ctrl + C' or 'Ctrl + Break'
								if ((bEnd = m_endEvent.WaitOne(100, false)) == true) {
									continue;
								}	//	end if
								if(simulationIndex%10 == 0){
									device1Node.Simulate();
								}	//	end if

								if (simulationIndex%25 == 0){
									device2Node.Simulate();
								}	//	end if

								simulationIndex++;

							}	//	end while
							Application.Instance.Stop();
						}
					}
					else {
						if (ret == (uint)EnumResultCode.S_FALSE) {
							System.Console.WriteLine("Registration operation {0}", Environment.CommandLine);
						}
					}
					Application.Instance.Terminate();        
				}
			}
			catch(Exception e){
				Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER, "console.main", "Exception"+e.ToString());
			}	//	end try ... catch
		}

		public static void BuildEventCategories(){
			
			//	we have a couple of events. Each class is responsible for building it's own cathegory
			DataEvent.BuildCategory();
			DeviceEvent.BuildCategory();
		}	//	end BuildEventCategories

		public static void BuildNamespace(){
						
			device1Node = new DaBaseElement();
			device1Node.HasChildren = true;
			device1Node.Name = "BMS1";
			Application.Instance.DaAddressSpaceRoot.AddChild(device1Node);

            ValueQT value = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
	        device1Node.AddProperty(new DaConstantProperty(6666, "HIERARCHY", "HIERARCHY", value));

			device1Node.AddChild(new DataPointElement("Datapoint11", 5002, "Meeting room 1"));
			device1Node.AddChild(new DataPointElement("Datapoint12", 5003, "Office 1"));
			device1Node.AddChild(new DataPointElement("Datapoint13", 5003, "Office 2"));
			device1Node.AddChild(new DataPointElement("Datapoint14", 5003, "Office 3"));
			device1Node.AddChild(new DataPointElement("Datapoint15", 5002, "Office 4"));
			device1Node.AddChild(new DataPointElement("Datapoint16", 5003, "Office 5"));
			device1Node.AddChild(new DeviceElement("DeviceState", 5001, "Device connection status datapoint"));

			device2Node = new DaBaseElement();
			device2Node.HasChildren = true;
			device2Node.Name = "BMS2";
			Application.Instance.DaAddressSpaceRoot.AddChild(device2Node);

            value = new ValueQT(0, EnumQuality.GOOD, DateTime.Now);
            device2Node.AddProperty(new DaConstantProperty(6666, "HIERARCHY", "HIERARCHY", value));
			
			device2Node.AddChild(new DataPointElement("Datapoint21", 5004, "Meeting room 1"));
			device2Node.AddChild(new DataPointElement("Datapoint22", 5004, "Office 1"));
			device2Node.AddChild(new DataPointElement("Datapoint23", 5004, "Office 2"));
			device2Node.AddChild(new DataPointElement("Datapoint24", 5004, "Office 3"));
			device2Node.AddChild(new DataPointElement("Datapoint25", 5005, "Office 4"));
			device2Node.AddChild(new DataPointElement("Datapoint26", 5005, "Office 5"));
			device2Node.AddChild(new DataPointElement("Datapoint27", 5004, "Office 5"));
			device2Node.AddChild(new DeviceElement("DeviceState", 5001, "Device connection status datapoint"));

			CommandElement cmd = new CommandElement();
			
			CommandProperty cmdProperty = new CommandProperty(5001, "DeviceState");
			cmdProperty.AddCommand("1", "Initialises Comms");
			cmdProperty.AddCommand("0", "Closes Comms");
			cmd.AddProperty(cmdProperty);
			
			cmdProperty = new CommandProperty(5002, "BMS1 5002");
			cmdProperty.AddCommand("3", "Init");
			cmdProperty.AddCommand("2", "Close");
			cmd.AddProperty(cmdProperty);
			
			cmdProperty = new CommandProperty(5003, "BMS1 5003");
			cmdProperty.AddCommand("5", "Resets sensor");
			cmdProperty.AddCommand("4", "Activates the sensor");
			cmd.AddProperty(cmdProperty);
			
			cmdProperty = new CommandProperty(5004, "BMS2 5004");
			cmdProperty.AddCommand("7", "Activate Zone");
			cmdProperty.AddCommand("6", "Deactivate Zone");
			cmd.AddProperty(cmdProperty);

			cmdProperty = new CommandProperty(5005, "BMS2 5005");
			cmdProperty.AddCommand("9", "Heating on");
			cmdProperty.AddCommand("8", "Heating off");
			cmd.AddProperty(cmdProperty);

			Application.Instance.DaAddressSpaceRoot.AddChild(cmd);			
		}	//	end BuildNamespace

		public static int Shutdown(){
			m_endEvent.Set();
			System.Console.WriteLine("Shutodwn indication received", Environment.CommandLine);
			return (int)EnumResultCode.S_OK;
		}	//	end Shutdown
	}

#endregion // Main class
    
}	//	end Softing.OPCToolbox.Server.BISDemo
