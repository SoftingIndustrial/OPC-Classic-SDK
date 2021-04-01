Imports System
Imports System.Threading
Imports Softing.OPCToolbox

Namespace DllSample
	''' <summary>
	''' Summary description for Dll.
	''' </summary>
	Public Class Dll
#Region "Private Attributes"
		'-------------------------
		Private m_simulationEnd As Boolean = False
		Private Shared m_mutex As New Mutex
		Private Shared m_opcServer As OpcServer = Nothing

		'--
#End Region

#Region "Private Methods"
		'----------------------

		Private Sub Simulate()
			' Wait until it is safe to enter.
			m_mutex.WaitOne()

			While Not m_simulationEnd
				'Simulate the AE Server events generation
				m_opcServer.FireSimulationEvents()
				'Simulate the DA Server data changes generation
				m_opcServer.ChangeSimulationValues()
				Thread.Sleep(1000)
			End While
			' Release the Mutex
			m_mutex.ReleaseMutex()

		End Sub

		Private Sub StartSimulationThread()
			Dim simulationThread As New Thread(AddressOf Simulate)
			simulationThread.Start()
		End Sub
		'--
#End Region

#Region "Public Methods"
		'--------------------

		Public Function Start() As Integer
			Try
				Dim result As Integer = 0
				Dim registrationResult As Integer = 0
				'  create and initialize the OpcServer instance
				m_opcServer = New OpcServer
				m_opcServer.Initialize()

				Dim creator As New MyCreator
				If Not ResultCode.SUCCEEDED(m_opcServer.Prepare(creator)) Then
					m_opcServer.Terminate()
					m_opcServer = Nothing
					Return CType(EnumResultCode.E_FAIL, Integer)
				End If

				'  handle the command line arguments (register/unregister, etc)
				Dim commandline As String = Environment.CommandLine
				registrationResult = m_opcServer.ProcessCommandLine(commandline)

				If Not (registrationResult = EnumResultCode.S_OK) Then
					If registrationResult = EnumResultCode.S_FALSE Then
						'registration operation succesful
						m_opcServer.Trace(EnumTraceLevel.INF, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "Dll::Main", "Registration succeeded")
					Else
						m_opcServer.Trace(EnumTraceLevel.INF, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "Console::Main", "Registration failed")
					End If

					'  no matter what close the application if
					'processCommandLine returned something different of S_OK        
					m_opcServer.Terminate()
					m_opcServer = Nothing
					Return CType(registrationResult, Integer)
				End If
				'  start the OPC server's I/O internal mechanism 
				If ResultCode.SUCCEEDED(m_opcServer.Start()) Then
					'  build the namespace
					m_opcServer.BuildAddressSpace()
					'  Build the event categories        
					m_opcServer.BuildEventCategories()
					'  declare the namespaces built and the server ready for clients to connect
					m_opcServer.Ready()
				End If
				'  start the simulation thread 
				If ResultCode.SUCCEEDED(result) Then
					StartSimulationThread()
				End If
				Return CType(registrationResult, Integer)
			Catch exc As Exception
				Console.WriteLine(exc.ToString())
				Return CType(EnumResultCode.E_FAIL, Integer)
			End Try
		End Function

		Public Sub [Stop]()
			'  terminate the simulation
			m_simulationEnd = True
			m_mutex.WaitOne()
			m_mutex.ReleaseMutex()
			m_opcServer.[Stop]()
			m_opcServer.Terminate()
			m_opcServer = Nothing
		End Sub

		'--
#End Region

	End Class
End Namespace

