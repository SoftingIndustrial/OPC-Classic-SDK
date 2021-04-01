Imports System
Imports System.Runtime.InteropServices
Imports System.Threading
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox

Namespace DaConsole
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
		Private Shared m_opcClient As OpcClient = Nothing
		'--
#End Region

#Region "Public Properties"
		'------------------------

		Public ReadOnly Property OpcClient() As OpcClient
			Get
				Return m_opcClient
			End Get
		End Property


		'--
#End Region

#Region "Public Methods"
		'--------------------------
		Public Sub CreateOpcClient()
			If m_opcClient Is Nothing Then
				m_opcClient = New OpcClient
			End If
		End Sub

		'--
#End Region

		Public Shared Sub Usage()

			System.Console.WriteLine()
			System.Console.Write("" & Chr(10) & "Usage:" & Chr(10) & "")
			System.Console.Write("Use lower case letters for synchronous execution " & Chr(10) & "")
			System.Console.Write("Use upper case letters for asynchronous execution " & Chr(10) & "")
			System.Console.Write("Press 'a' or 'A' to activate the objects" & Chr(10) & "")
			System.Console.Write("Press 'c' or 'C' to connect the objects" & Chr(10) & "")
			System.Console.Write("Press 'd' or 'D' to disconnect the objects" & Chr(10) & "")
			System.Console.Write("Press 'r' or 'R' to read the items" & Chr(10) & "")
			System.Console.Write("Press 'w' or 'W' to write the items" & Chr(10) & "")
			System.Console.Write("Press 'z' or 'Z' to read using server object" & Chr(10) & "")
			System.Console.Write("Press 't' or 'T'to write using the server object" & Chr(10) & "")
            System.Console.Write("Press 's' or 'S' to get the server status" & Chr(10) & "")
            System.Console.Write("Press 'h' or 'H'to change the subscription`s data retrieval mode" & Chr(10) & "")
			System.Console.Write("Press 'l' or 'L' to log in" & Chr(10) & "")
			System.Console.Write("Press 'm' to activate the connection monitor" & Chr(10) & "")
			System.Console.Write("Press 'n' to deactivate the connection monitor" & Chr(10) & "")
			System.Console.Write("Press '?' or ""u"" to display this usage information" & Chr(10) & "")
			System.Console.Write("Press 'e' or 'q' or Ctrl-C to exit" & Chr(10) & "")
			System.Console.WriteLine()
		End Sub

		Public Shared Sub Main(ByVal args As String())

			Try
				Dim result As Integer = EnumResultCode.S_OK
				EndEvent = New AutoResetEvent(False)
				Dim console As New Console

				Dim handlerRoutine As New MyWin32.HandlerRoutine(AddressOf MyWin32.Handler)
				MyWin32.SetConsoleCtrlHandler(handlerRoutine, True)

				console.CreateOpcClient()
				' gets the OpcClient instance
				Dim m_opcClient As OpcClient = console.OpcClient

				'  initialize the client instance
				If Not ResultCode.SUCCEEDED(m_opcClient.Initialize()) Then
					m_opcClient = Nothing
					Return
				End If

				'  initialize the DA client simulation
				result = result Or m_opcClient.InitializeDaObjects()

				Usage()

				Dim [end] As Boolean = False
				While Not [end] AndAlso Not Console.[End]
					Dim read As String = System.Console.ReadLine()
					Select Case read
						Case "A"
							m_opcClient.ActivateSession(False)
						Case "a"
							m_opcClient.ActivateSession(True)
						Case "C"
							m_opcClient.ConnectSession(False)
						Case "c"
							m_opcClient.ConnectSession(True)
						Case "D"
							m_opcClient.DisconnectSession(False)
						Case "d"
							m_opcClient.DisconnectSession(True)
						Case "R"
							m_opcClient.ReadItems(False)
						Case "r"
							m_opcClient.ReadItems(True)
						Case "W"
							m_opcClient.WriteItems(False)
						Case "w"
							m_opcClient.WriteItems(True)
						Case "Z"
							m_opcClient.ReadUsingSession(False)
						Case "z"
							m_opcClient.ReadUsingSession(True)
						Case "T"
							m_opcClient.WriteUsingSession(False)
						Case "t"
							m_opcClient.WriteUsingSession(True)
						Case "S"
							m_opcClient.GetServerStatus(False)
						Case "s"
							m_opcClient.GetServerStatus(True)
						Case "M", "m"
							m_opcClient.ActivateConnectionMonitor()
						Case "N", "n"
							m_opcClient.DeactivateConnectionMonitor()
						Case "l"
							m_opcClient.SetCredentials()
							m_opcClient.LoginSecure(True)
						Case "L"
							m_opcClient.SetCredentials()
							m_opcClient.LoginSecure(False)
						Case "o"
							m_opcClient.LogoffSecure(True)
							m_opcClient.SetCredentials("Guest", "nopass")
						Case "O"
							m_opcClient.LogoffSecure(False)
                            m_opcClient.SetCredentials("Guest", "nopass")
                        Case "h", "H"
                            m_opcClient.ChangeDataRetrievalMode()
                        Case "U", "u", "?"
                            Usage()

						Case "E", "e", "Q", "q"
							[end] = True

						Case Else
							Usage()
					End Select
				End While


				m_opcClient.Terminate()
				m_opcClient = Nothing
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

