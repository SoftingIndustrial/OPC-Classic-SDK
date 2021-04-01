Imports System
Imports System.Collections
Imports System.Runtime.InteropServices
Imports System.Threading
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server
Imports Microsoft.VisualBasic

Namespace Session
	Public Class Console
#Region "Public Attributes"
		'-----------------------------------

		' signals application should terminate
		Public Shared EndEvent As AutoResetEvent
		Public Shared [End] As Boolean = False
		Public Shared m_clientChanged As AutoResetEvent
		Public Shared m_sessions As New ArrayList
		Public Shared m_sessionsJanitor As New Object
		'--
#End Region

#Region "Private Attributes"
		'-------------------------
		Private Shared m_opcServer As OpcServer = Nothing
		'--
#End Region

#Region "Public Properties"
		'------------------------

		Public ReadOnly Property OpcServer() As OpcServer
			Get
				Return m_opcServer
			End Get
		End Property
		'--
#End Region

#Region "Public Methods"
		'--------------------------
		Public Sub CreateOpcServer()
			If m_opcServer Is Nothing Then
				m_opcServer = New OpcServer
			End If
		End Sub

		'--
#End Region

		Public Shared Sub Main(ByVal args As String())
			Try
				Dim result As Integer = EnumResultCode.S_OK
				EndEvent = New AutoResetEvent(False)
				m_clientChanged = New AutoResetEvent(True)
				Dim console As New Console

				Dim handlerRoutine As New MyWin32.HandlerRoutine(AddressOf MyWin32.Handler)
				MyWin32.SetConsoleCtrlHandler(handlerRoutine, True)

				Dim m_traceGroupUser1 As EnumTraceGroup
				m_traceGroupUser1 = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1)

				'  create and initialize the OpcServer instance
				console.CreateOpcServer()
				Dim server As OpcServer = console.OpcServer
				server.Initialize()

				Dim creator As New MyCreator
				If Not ResultCode.SUCCEEDED(server.Prepare(creator)) Then
					server.Terminate()
					MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT)
					server = Nothing
					Return
				End If

				'  handle the command line arguments (register/unregister, etc)
				Dim commandline As String = Environment.CommandLine
				result = server.ProcessCommandLine(commandline)

				If result <> EnumResultCode.S_OK Then
					If result = EnumResultCode.S_FALSE Then
						'registration operation succesful
						server.Trace(EnumTraceLevel.INF, m_traceGroupUser1, "Console::Main", "Registration succeeded")
					Else
						server.Trace(EnumTraceLevel.INF, m_traceGroupUser1, "Console::Main", "Registration failed")
					End If

					'  no matter what close the application if
					'processCommandLine returned something different of S_OK
					server.Terminate()
					server = Nothing
					Return
				End If

				'  start the OPC server's I/O internal mechanism 
				If ResultCode.SUCCEEDED(server.Start()) Then
					server.BuildAddressSpace()
					'  declare the namespaces built and the server ready for clients to connect
					server.Ready()
				End If
				System.Console.WriteLine("Press Ctrl-C to exit" & Chr(10) & "")
				Dim iRet As Int32 = WaitHandle.WaitTimeout

				While Not Console.[End]
					' wait for keyboard entry 'Ctrl + C' or 'Ctrl + Break'
					iRet = System.Threading.WaitHandle.WaitAny(New AutoResetEvent() {EndEvent, m_clientChanged}, -1, True)
					If iRet = 0 Then
						[End] = True
					Else
						ShowObjectTree()
					End If

					Thread.Sleep(100)
				End While


				server.[Stop]()
				server.Terminate()
				server = Nothing
				MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT)
			Catch exc As Exception
				System.Console.WriteLine(exc.ToString())
			End Try
		End Sub

		Shared Sub ShowObjectTree()

			ClearConsole.Clear()

			System.Console.WriteLine("CLIENTS:" & Chr(10) & "")

			SyncLock m_sessionsJanitor
				For Each session As MySession In m_sessions

					Dim szType As String
					Select Case session.Type
						Case EnumSessionType.DA
							szType = "DA"
						Case EnumSessionType.XMLDA
							szType = "XMLDA"
						Case EnumSessionType.XMLSUBSCRIPTIONS
							szType = "INTERNAL XML-DA"
						Case Else
							szType = "Unknown Type"
					End Select

					System.Console.WriteLine(String.Format("{0,0:X}" & Chr(9) & "{1}" & Chr(9) & "{2}", session.Handle, szType, IIf((session Is MySession.s_controlingSession), "<IN CONTROL>", "")))

				Next
			End SyncLock

			System.Console.WriteLine("" & Chr(10) & "" & Chr(10) & "ITEMS:" & Chr(10) & "")
			For Each item As MyDaAddressSpaceElement In Application.Instance.DaAddressSpaceRoot.GetChildren()

				If Not item Is Nothing Then
					System.Console.WriteLine(String.Format("{0}" & Chr(9) & "" & Chr(9) & "{1}" & Chr(9) & "{2}", item.Name, item.Active.ToString(), item.UpdateRate))
				End If
			Next

			System.Console.WriteLine("" & Chr(10) & "" & Chr(10) & "Press Ctrl-C to exit")

		End Sub

	End Class

	Public Class MyWin32
		' Declare the SetConsoleCtrlHandler function 
		' as external and receiving a delegate.   
		<DllImport("Kernel32")> _
		Public Shared Function SetConsoleCtrlHandler(ByVal Handler As HandlerRoutine, ByVal Add As Boolean) As Boolean
		End Function

		' A delegate type to be used as the handler routine 
		' for SetConsoleCtrlHandler.
		Public Delegate Function HandlerRoutine(ByVal CtrlType As CtrlTypes) As Boolean

		' An enumerated type for the control messages 
		' sent to the handler routine.
		Public Enum CtrlTypes
			CTRL_C_EVENT = 0
			CTRL_BREAK_EVENT
			CTRL_CLOSE_EVENT
			CTRL_LOGOFF_EVENT = 5
			CTRL_SHUTDOWN_EVENT
		End Enum

		' A private static handler function.
		Public Shared Function Handler(ByVal CtrlType As MyWin32.CtrlTypes) As Boolean
			Dim message As String = String.Empty

			' A switch to handle the event type.
			Select Case CtrlType
				Case MyWin32.CtrlTypes.CTRL_C_EVENT, MyWin32.CtrlTypes.CTRL_BREAK_EVENT, MyWin32.CtrlTypes.CTRL_CLOSE_EVENT, MyWin32.CtrlTypes.CTRL_LOGOFF_EVENT, MyWin32.CtrlTypes.CTRL_SHUTDOWN_EVENT
					message = "Stop execution, since CTRL command!"
					Console.EndEvent.[Set]()
					Console.[End] = True
			End Select
			' Use interop to display a message for the type of event.
			System.Console.WriteLine(message)
			Return True
		End Function
	End Class

	Public Class ClearConsole
		Private Const STD_OUTPUT_HANDLE As Integer = -11
		Private Const EMPTY As Byte = 32

		<StructLayout(LayoutKind.Sequential)> _
		Structure COORD
			Public x As Short
			Public y As Short
		End Structure

		<StructLayout(LayoutKind.Sequential)> _
		Structure SMALL_RECT
			Public Left As Short
			Public Top As Short
			Public Right As Short
			Public Bottom As Short
		End Structure

		<StructLayout(LayoutKind.Sequential)> _
		Structure CONSOLE_SCREEN_BUFFER_INFO
			Public dwSize As COORD
			Public dwCursorPosition As COORD
			Public wAttributes As Integer
			Public srWindow As SMALL_RECT
			Public dwMaximumWindowSize As COORD
		End Structure

		<DllImport("kernel32.dll", EntryPoint:="GetStdHandle", SetLastError:=True, CharSet:=CharSet.Ansi, CallingConvention:=CallingConvention.StdCall)> _
		Private Shared Function GetLastError() As Integer
		End Function

		<DllImport("kernel32.dll", EntryPoint:="GetStdHandle", SetLastError:=True, CharSet:=CharSet.Ansi, CallingConvention:=CallingConvention.StdCall)> _
		Private Shared Function GetStdHandle(ByVal nStdHandle As Integer) As Integer
		End Function

		<DllImport("kernel32.dll", EntryPoint:="FillConsoleOutputCharacter", SetLastError:=True, CharSet:=CharSet.Ansi, CallingConvention:=CallingConvention.StdCall)> _
		Private Shared Function FillConsoleOutputCharacter(ByVal hConsoleOutput As Integer, ByVal cCharacter As Byte, ByVal nLength As Integer, ByVal dwWriteCoord As COORD, ByRef lpNumberOfCharsWritten As Integer) As Integer
		End Function

		<DllImport("kernel32.dll", EntryPoint:="GetConsoleScreenBufferInfo", SetLastError:=True, CharSet:=CharSet.Ansi, CallingConvention:=CallingConvention.StdCall)> _
		Private Shared Function GetConsoleScreenBufferInfo(ByVal hConsoleOutput As Integer, ByRef lpConsoleScreenBufferInfo As CONSOLE_SCREEN_BUFFER_INFO) As Integer
		End Function

		<DllImport("kernel32.dll", EntryPoint:="SetConsoleCursorPosition", SetLastError:=True, CharSet:=CharSet.Ansi, CallingConvention:=CallingConvention.StdCall)> _
		Private Shared Function SetConsoleCursorPosition(ByVal hConsoleOutput As Integer, ByVal dwCursorPosition As COORD) As Integer
		End Function

		Private hConsoleHandle As Integer

		Public Sub New()
			' 
			' TODO: Add constructor logic here.
			' 
			hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE)
		End Sub

		Public Shared Sub Clear()
			Dim iRet As Integer
			Dim hConsoleHandle As Integer = GetStdHandle(STD_OUTPUT_HANDLE)

			Dim hWrittenChars As Integer = 0
			Dim strConsoleInfo As New CONSOLE_SCREEN_BUFFER_INFO
			Dim Home As COORD
			Home.x = Home.y = 0
			iRet = GetConsoleScreenBufferInfo(hConsoleHandle, strConsoleInfo)
			iRet = FillConsoleOutputCharacter(hConsoleHandle, EMPTY, strConsoleInfo.dwSize.x * strConsoleInfo.dwSize.y, Home, hWrittenChars)
			iRet = SetConsoleCursorPosition(hConsoleHandle, Home)
		End Sub
	End Class

End Namespace

