using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Console
{
	public class Console
	{
		#region Public Attributes
		//-----------------------------------

		// signals application should terminate
		public static AutoResetEvent EndEvent;
		public static bool End = false;

		//--
		#endregion

		#region Private Attributes
		//-------------------------
		private static OpcServer m_opcServer = null;
		//--
		#endregion

		#region Public Properties
		//------------------------

		public OpcServer OpcServer
		{
			get
			{
				return m_opcServer;
			}   //	end get
		}   //	end OpcServer
			//--
		#endregion

		#region Public Methods
		//--------------------------
		public void CreateOpcServer()
		{
			if (m_opcServer == null)
			{
				m_opcServer = new OpcServer();
			}   //	em=nd if
		}   //	end CreateOpcClient

		//--
		#endregion

		public static void Main(String[] args)
		{
			try
			{
				int result = (int)EnumResultCode.S_OK;
				EndEvent = new AutoResetEvent(false);
				Console console = new Console();

				MyWin32.HandlerRoutine handlerRoutine = new MyWin32.HandlerRoutine(MyWin32.Handler);
				MyWin32.SetConsoleCtrlHandler(
					handlerRoutine,
					true);


				//	create and initialize the OpcServer instance
				console.CreateOpcServer();
				OpcServer server = console.OpcServer;
				server.Initialize();

				MyCreator creator = new MyCreator();
				if (!ResultCode.SUCCEEDED(server.Prepare(creator)))
				{
					server.Terminate();
					MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
					server = null;
					return;
				}   //	end if

				//	handle the command line arguments (register/unregister, etc)
				string commandline = Environment.CommandLine;
				result = server.ProcessCommandLine(commandline);

				if (result != (uint)EnumResultCode.S_OK)
				{
					if (result == (uint)EnumResultCode.S_FALSE)
					{
						//registration operation succesful
						server.Trace(
							EnumTraceLevel.INF,
							EnumTraceGroup.USER1,
							"Console::Main",
							"Registration succeeded");
					}
					else
					{
						server.Trace(
							EnumTraceLevel.INF,
							EnumTraceGroup.USER1,
							"Console::Main",
							"Registration failed");
					}   //	end if...else

					//	no matter what close the application if
					//processCommandLine returned something different of S_OK
					server.Terminate();
					server = null;
					return;
				}   //	end if

				//	start the OPC server's I/O internal mechanism 
				if (ResultCode.SUCCEEDED(server.Start()))
				{
					server.BuildEventCategories();
					server.BuildAddressSpace();
					server.InitializeConditions();
					//	declare the namespaces built and the server ready for clients to connect
					server.Ready();
				}   //	end if


				System.Console.WriteLine("Press Ctrl-C to exit\n");

				bool firstLoop = true;
				DateTime dt = DateTime.UtcNow;
				int second = 0xFF;
				int minute = 0xFF;
				int hour = 0xFF;

				while (!Console.End)
				{
					Thread.Sleep(1000);
					DateTime dateTimeNow = DateTime.UtcNow;

					server.CheckValuesChanged(ref dt);

					ArrayList conditionList = new ArrayList();
					if (!firstLoop)
					{
						if (second != dateTimeNow.Second)
						{
							switch (dateTimeNow.Second)
							{
								case 10:
									server.AtSecond10(ref conditionList);
									break;

								case 30:
									server.AtSecond30(ref conditionList);
									break;

								case 50:
									server.AtSecond50(ref conditionList);
									break;
							}   //	end switch
						}
					}
					else
					{
						firstLoop = false;
						if ((dateTimeNow.Second >= 10) && (dateTimeNow.Second < 30))
						{
							server.BetweenSecond10and30(ref conditionList);

						}   //	end if

						if ((dateTimeNow.Second >= 30) && (dateTimeNow.Second < 50))
						{
							server.BetweenSecond30and50(ref conditionList);
						}   //	end if
					}   //	end if ... else

					if (conditionList.Count > 0)
					{
						Application.Instance.ChangeConditions(conditionList);
					}   //	end if

					// simple events
					ArrayList events = new ArrayList();
					ArrayList attributeValues = new ArrayList();
					attributeValues.Add(dateTimeNow.Hour);
					attributeValues.Add(dateTimeNow.Minute);
					attributeValues.Add(dateTimeNow.Second);

					if (second != dateTimeNow.Second)
					{
						second = dateTimeNow.Second;

						server.AddSimpleEventSecond(ref events, attributeValues);
					}// end if

					if (minute != dateTimeNow.Minute)
					{
						minute = dateTimeNow.Minute;

						server.AddSimpleEventMinute(ref events, attributeValues);
					}   //	end if

					if (hour != DateTime.Now.Hour)
					{
						hour = DateTime.Now.Hour;

						server.AddSimpleEventHour(ref events, attributeValues);
					}   //	end if

					Application.Instance.FireEvents(events);
				}   //	end while

				server.Stop();
				server.Terminate();
				server = null;
				MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
			}
			catch (Exception exc)
			{
				Application.Instance.Trace(EnumTraceLevel.ERR, EnumTraceGroup.OPCSERVER, "console.main", "Exception" + exc.ToString());
			}   //	end try...catch
		}   //	end Main
	}   //	end class Console

	public class MyWin32
	{
		// Declare the SetConsoleCtrlHandler function 
		// as external and receiving a delegate.   
		[DllImport("Kernel32")]
		public static extern Boolean SetConsoleCtrlHandler(
			HandlerRoutine Handler,
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
		}   //	end enum CtrlTypes

		// A private static handler function.
		static public Boolean Handler(MyWin32.CtrlTypes CtrlType)
		{
			string message = string.Empty;

			// A switch to handle the event type.
			switch (CtrlType)
			{
				case MyWin32.CtrlTypes.CTRL_C_EVENT:
				case MyWin32.CtrlTypes.CTRL_BREAK_EVENT:
				case MyWin32.CtrlTypes.CTRL_CLOSE_EVENT:
				case MyWin32.CtrlTypes.CTRL_LOGOFF_EVENT:
				case MyWin32.CtrlTypes.CTRL_SHUTDOWN_EVENT:
					message = "Stop execution, since CTRL command!";
					Console.EndEvent.Set();
					Console.End = true;
					break;
			}   //	end switch
				// Use interop to display a message for the type of event.
			System.Console.WriteLine(message);
			return true;
		}   //	end Handler
	}   //	end class Console
}   //	end namespace
