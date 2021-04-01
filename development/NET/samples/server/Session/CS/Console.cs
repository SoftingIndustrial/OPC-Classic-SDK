using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Threading;
using Softing.OPCToolbox;
using Softing.OPCToolbox.Server;

namespace Session
{
	public class Console
	{
		#region Public Attributes
		//-----------------------------------

		// signals application should terminate
		public static AutoResetEvent EndEvent;
		public static bool End = false;
		public static AutoResetEvent m_clientChanged;
		public static ArrayList m_sessions = new ArrayList();
		public static object m_sessionsJanitor = new object();
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
				m_clientChanged = new AutoResetEvent(true);
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
					server.BuildAddressSpace();
					//	declare the namespaces built and the server ready for clients to connect
					server.Ready();
				}   //	end if
				System.Console.WriteLine("Press Ctrl-C to exit\n");
				Int32 iRet = WaitHandle.WaitTimeout;

				while (!Console.End)
				{
					// wait for keyboard entry 'Ctrl + C' or 'Ctrl + Break'
					iRet = System.Threading.WaitHandle.WaitAny(new AutoResetEvent[] { EndEvent, m_clientChanged }, -1, true);
					if (iRet == 0)
					{
						End = true;
					}
					else
					{
						ShowObjectTree();
					}
				}   //	end while


				server.Stop();
				server.Terminate();
				server = null;
				MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT);
			}
			catch (Exception exc)
			{
				System.Console.WriteLine(exc.ToString());
			}   //	end try...catch
		}   //	end Main

		static void ShowObjectTree()
		{

			ClearConsole.Clear();

			System.Console.WriteLine("CLIENTS:\n");

			lock (m_sessionsJanitor)
			{
				foreach (MySession session in Console.m_sessions)
				{

					string szType;
					switch (session.Type)
					{
						case EnumSessionType.DA:
							szType = "DA";
							break;
						case EnumSessionType.XMLDA:
							szType = "XMLDA";
							break;
						case EnumSessionType.XMLSUBSCRIPTIONS:
							szType = "INTERNAL XML-DA";
							break;
						default:
							szType = "Unknown Type";
							break;
					}   //	end switch

					System.Console.WriteLine(string.Format("{0,0:X}\t{1}\t{2}", session.Handle, szType, (session == MySession.s_controlingSession) ? "<IN CONTROL>" : ""));

				}   //	end foreach
			}   //	end lock

			System.Console.WriteLine("\n\nITEMS:\n");
			foreach (MyDaAddressSpaceElement item in Application.Instance.DaAddressSpaceRoot.GetChildren())
			{

				if (item != null)
				{
					System.Console.WriteLine(string.Format("{0}\t\t{1}\t{2}", item.Name, item.Active.ToString(), item.UpdateRate));
				}   //	end if
			}   //	end foreach

			System.Console.WriteLine("\n\nPress Ctrl-C to exit");

		}   //	end ShowObjectTree

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

	public class ClearConsole
	{
		private const int STD_OUTPUT_HANDLE = -11;
		private const byte EMPTY = 32;

		[StructLayout(LayoutKind.Sequential)]
		struct COORD
		{
			public short x;
			public short y;
		}

		[StructLayout(LayoutKind.Sequential)]
		struct SMALL_RECT
		{
			public short Left;
			public short Top;
			public short Right;
			public short Bottom;
		}

		[StructLayout(LayoutKind.Sequential)]
		struct CONSOLE_SCREEN_BUFFER_INFO
		{
			public COORD dwSize;
			public COORD dwCursorPosition;
			public int wAttributes;
			public SMALL_RECT srWindow;
			public COORD dwMaximumWindowSize;
		}

		[DllImport("kernel32.dll", EntryPoint = "GetStdHandle", SetLastError = true, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
		private static extern int GetLastError();

		[DllImport("kernel32.dll", EntryPoint = "GetStdHandle", SetLastError = true, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
		private static extern int GetStdHandle(int nStdHandle);

		[DllImport("kernel32.dll", EntryPoint = "FillConsoleOutputCharacter", SetLastError = true, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
		private static extern int FillConsoleOutputCharacter(int hConsoleOutput, byte cCharacter, int nLength, COORD dwWriteCoord, ref int lpNumberOfCharsWritten);

		[DllImport("kernel32.dll", EntryPoint = "GetConsoleScreenBufferInfo", SetLastError = true, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
		private static extern int GetConsoleScreenBufferInfo(int hConsoleOutput, ref CONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo);

		[DllImport("kernel32.dll", EntryPoint = "SetConsoleCursorPosition", SetLastError = true, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
		private static extern int SetConsoleCursorPosition(int hConsoleOutput, COORD dwCursorPosition);

		private int hConsoleHandle;

		public ClearConsole()
		{
			// 
			// TODO: Add constructor logic here.
			// 
			hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		}

		public static void Clear()
		{
			int iRet;
			int hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

			int hWrittenChars = 0;
			CONSOLE_SCREEN_BUFFER_INFO strConsoleInfo = new CONSOLE_SCREEN_BUFFER_INFO();
			COORD Home;
			Home.x = Home.y = 0;
			iRet = GetConsoleScreenBufferInfo(hConsoleHandle, ref strConsoleInfo);
			iRet = FillConsoleOutputCharacter(hConsoleHandle, EMPTY, strConsoleInfo.dwSize.x * strConsoleInfo.dwSize.y, Home, ref hWrittenChars);
			iRet = SetConsoleCursorPosition(hConsoleHandle, Home);
		}
	} // end class ClearConsole

}   //	end namespace
