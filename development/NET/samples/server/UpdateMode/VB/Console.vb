Imports System
Imports System.Runtime.InteropServices
Imports System.Threading
Imports Softing.OPCToolbox

Namespace UpdateMode
	Public Class Console
#Region "Public Attributes"
		'-----------------------------------

		' signals application should terminate
		Public Shared EndEvent As AutoResetEvent
		Public Shared [End] As Boolean = False
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
				m_opcServer = New OpcServer()
			End If
		End Sub

		'--
#End Region

		Public Shared Sub Main(ByVal args As String())
			Try
				Dim result As Integer = 0
				EndEvent = New AutoResetEvent(False)
				Dim console As New Console()

				Dim handlerRoutine As MyWin32.HandlerRoutine
				handlerRoutine = AddressOf MyWin32.Handler
				MyWin32.SetConsoleCtrlHandler(handlerRoutine, True)

				'  create and initialize the OpcServer instance
				console.CreateOpcServer()
				Dim server As OpcServer = console.OpcServer
				server.Initialize()

				Dim creator As New MyCreator()
				If Not ResultCode.SUCCEEDED(server.Prepare(creator)) Then
					server.Terminate()
					MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT)
					server = Nothing
					Return
				End If

				'  handle the command line arguments (register/unregister, etc)
				Dim commandline As String = Environment.CommandLine
				result = server.ProcessCommandLine(commandline)

				If Not (result = EnumResultCode.S_OK) Then
					If result = EnumResultCode.S_FALSE Then
						'registration operation succesful
						server.Trace(EnumTraceLevel.INF, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "Console::Main", "Registration succeeded")
					Else
						server.Trace(EnumTraceLevel.INF, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "Console::Main", "Registration failed")
					End If

					'  no matter what close the application if
					'processCommandLine returned something different of S_OK
					server.Terminate()
					server = Nothing
					Return
				End If

				'  start the OPC server's I/O internal mechanism 
				If ResultCode.SUCCEEDED(server.Start()) Then
					'  build the namespace
					server.BuildAddressSpace()
					'  declare the namespaces built and the server ready for clients to connect
					server.Ready()
				End If

				System.Console.WriteLine("Press Ctrl-C to exit ! " & Microsoft.VisualBasic.Chr(10))

				Dim now As DateTime = DateTime.Now
				Dim second As Integer = 255

				m_opcServer.ChangeValue(now.Second, EnumDaVariableType.SEC)
				m_opcServer.ChangeValue(now.Minute, EnumDaVariableType.MIN)

				While Not Console.[End]
					now = DateTime.Now

					If second <> now.Second Then
						Dim currentSecond As Integer = now.Second
						m_opcServer.ChangeValue(currentSecond, EnumDaVariableType.SEC)

						If currentSecond = 0 Then
							m_opcServer.ChangeValue(now.Minute, EnumDaVariableType.MIN)
						End If
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
End Namespace

