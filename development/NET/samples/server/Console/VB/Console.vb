'-----------------------------------------------------------------------------
'                                                                            |
'                   Softing Industrial Automation GmbH                       |
'                        Richard-Reitzner-Allee 6                            |
'                           85540 Haar, Germany                              |
'                                                                            |
'                 This is a part of the Softing OPC Toolkit                  |
'        Copyright (c) 2005-2020 Softing Industrial Automation GmbH          |
'                           All Rights Reserved                              |
'                                                                            |
'-----------------------------------------------------------------------------
'-----------------------------------------------------------------------------
'                             OPC Toolkit NET                                |
'                                                                            |
'  Filename    : Console.vb                                                  |
'  Version     : 4.47.1                                                      |
'  Date        : 27-July-2020                                                |
'                                                                            |
'  Description : Console application main implementation                     |
'                                                                            |
'-----------------------------------------------------------------------------
Imports System
Imports System.Collections
Imports System.Runtime.InteropServices
Imports System.Threading
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Console
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
				m_opcServer = New OpcServer
			End If
		End Sub

		'--
#End Region

		Public Shared Sub Main(ByVal args As String())
			Try
				Dim result As Integer = EnumResultCode.S_OK
				EndEvent = New AutoResetEvent(False)
				Dim console As New Console

				Dim handlerRoutine As New MyWin32.HandlerRoutine(AddressOf MyWin32.Handler)
				MyWin32.SetConsoleCtrlHandler(handlerRoutine, True)

				Dim traceGroupUser1 As EnumTraceGroup
				traceGroupUser1 = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1)


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
						server.Trace(EnumTraceLevel.INF, traceGroupUser1, "Console::Main", "Registration succeeded")
					Else
						server.Trace(EnumTraceLevel.INF, traceGroupUser1, "Console::Main", "Registration failed")
					End If

					'  no matter what close the application if
					'processCommandLine returned something different of S_OK
					server.Terminate()
					server = Nothing
					Return
				End If

				'  start the OPC server's I/O internal mechanism 
				If ResultCode.SUCCEEDED(server.Start()) Then

					server.BuildEventCategories()
					server.BuildAddressSpace()
					server.InitializeConditions()

					'  declare the namespaces built and the server ready for clients to connect
					server.Ready()
				End If

				System.Console.WriteLine("Press Ctrl-C to exit" & Chr(10) & "")

				Dim firstLoop As Boolean = True
				Dim dt As DateTime = DateTime.UtcNow
				Dim second As Integer = 255
				Dim minute As Integer = 255
				Dim hour As Integer = 255

				While Not Console.[End]
					Thread.Sleep(100)
					Dim dateTimeNow As DateTime = DateTime.UtcNow

					server.CheckValuesChanged(dt)

					Dim conditionList As New ArrayList
					If Not firstLoop Then
						If second <> dateTimeNow.Second Then
							Select Case dateTimeNow.Second
								Case 10
									server.AtSecond10(conditionList)

								Case 30
									server.AtSecond30(conditionList)

								Case 50
									server.AtSecond50(conditionList)
							End Select
						End If
					Else
						firstLoop = False
						If (dateTimeNow.Second >= 10) AndAlso (dateTimeNow.Second < 30) Then
							server.BetweenSecond10and30(conditionList)

						End If

						If (dateTimeNow.Second >= 30) AndAlso (dateTimeNow.Second < 50) Then
							server.BetweenSecond30and50(conditionList)
						End If
					End If

					If conditionList.Count > 0 Then
						Application.Instance.ChangeConditions(conditionList)
					End If

					' simple events
					Dim events As New ArrayList
					Dim attributeValues As New ArrayList
					attributeValues.Add(dateTimeNow.Hour)
					attributeValues.Add(dateTimeNow.Minute)
					attributeValues.Add(dateTimeNow.Second)

					If second <> dateTimeNow.Second Then
						second = dateTimeNow.Second

						server.AddSimpleEventSecond(events, attributeValues)
					End If

					If minute <> dateTimeNow.Minute Then
						minute = dateTimeNow.Minute

						server.AddSimpleEventMinute(events, attributeValues)
					End If

					If hour <> DateTime.Now.Hour Then
						hour = DateTime.Now.Hour

						server.AddSimpleEventHour(events, attributeValues)
					End If

					Application.Instance.FireEvents(events)
				End While

				server.[Stop]()
				server.Terminate()
				server = Nothing
				MyWin32.Handler(MyWin32.CtrlTypes.CTRL_CLOSE_EVENT)
			Catch exc As Exception
				Dim traceGroupOpcServer As EnumTraceGroup
				traceGroupOpcServer = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.OPCSERVER)
				Application.Instance.Trace(EnumTraceLevel.ERR, traceGroupOpcServer, "console.main", "Exception" + exc.ToString())
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

