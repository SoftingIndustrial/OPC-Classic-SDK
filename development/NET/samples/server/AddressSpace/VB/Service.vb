Imports System
Imports System.Threading
Imports System.IO
Imports Softing.OPCToolbox

Namespace AddressSpace

	Public Class Service
		Inherits System.ServiceProcess.ServiceBase

#Region "Private Attributes"

		Private m_simulationEnd As Boolean = False
		Private Shared m_mutex As New Mutex
		Private m_count As Integer = 0
		Private Shared m_opcServer As OpcServer = Nothing
		' The following constant holds the name and the description of the Windows NT service
		' the OPC application runs under.
		'    TODO : change your service name and description here
		Private Shared defaultServiceName As String = "Softing.OPCToolbox.NET.VB.AddressSpace"
		Private Shared defaultServiceDescription As String = "Softing OPC Toolkit - OPC DA Server. " & Environment.NewLine & Environment.NewLine & "Sample name: ""VB AddressSpace_Service"""

#End Region

#Region "Public Properties"
		'------------------------

		Public Property SimulationEnd() As Boolean
			Get
				Return m_simulationEnd
			End Get
			Set(ByVal value As Boolean)
				m_simulationEnd = value
			End Set
		End Property

		Public ReadOnly Property Mutex() As Mutex
			Get
				Return m_mutex
			End Get
		End Property

		Public ReadOnly Property OpcServer() As OpcServer
			Get
				Return m_opcServer
			End Get
		End Property

		'--
#End Region

#Region "Public Methods"
		'--------------------

		Public Sub CreateOpcServer()
			If m_opcServer Is Nothing Then
				m_opcServer = New OpcServer
			End If
		End Sub

		Public Sub Simulate()
			' Wait until it is safe to enter.
			m_mutex.WaitOne()

			While Not m_simulationEnd
				If m_count > 30 Then

					OpcServer.ReconfigHandles(0).RemoveChild(OpcServer.ReconfigHandles(2))
					OpcServer.ObjectBased.RemoveChild(OpcServer.ReconfigHandles(0))
					OpcServer.ObjectBased.RemoveChild(OpcServer.ReconfigHandles(1))
					m_opcServer.RebuildRandomAddressSpace(OpcServer.ObjectBased)
					OpcServer.AddressSpaceChanged.[Set]()
					m_count = 0
				Else
					System.Math.Max(System.Threading.Interlocked.Increment(m_count), m_count - 1)
				End If

				Thread.Sleep(1000)
			End While
			' Release the Mutex
			m_mutex.ReleaseMutex()
		End Sub

		Public Sub StartSimulationThread()
			Dim simulationThread As Thread = New Thread(AddressOf Simulate)
			simulationThread.Start()
		End Sub
		'--
#End Region

		Public Sub New()
		End Sub

		' The main entry point for the process
		Shared Sub Main()
			Dim result As Integer = 0
			Dim commandline As String = Environment.CommandLine

			AddressSpace.OpcServer.AddressSpaceChanged = New System.Threading.AutoResetEvent(True)

			Dim ServicesToRun As System.ServiceProcess.ServiceBase()
			Dim opcService As New Service
			ServicesToRun = New System.ServiceProcess.ServiceBase() {opcService}

			'  create and initialize the OpcServer instance
			opcService.CreateOpcServer()
			m_opcServer.Initialize()
			m_opcServer.ServiceName = defaultServiceName
			m_opcServer.ServiceDescription = defaultServiceDescription

			m_opcServer.ReconfigHandles = New MyDaAddressSpaceElement(3) {}
			Dim creator As New MyCreator
			If Not ResultCode.SUCCEEDED(m_opcServer.Prepare(creator)) Then
				m_opcServer.Terminate()
				m_opcServer = Nothing
				Return
			End If

			'  handle the command line arguments (register/unregister, etc)      
			result = m_opcServer.ProcessCommandLine(commandline)
			Dim traceGroup As EnumTraceGroup
			traceGroup = CType([Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), EnumTraceGroup)

			If Not (result = EnumResultCode.S_OK) Then
				If result = EnumResultCode.S_FALSE Then
					'registration operation succesful
					m_opcServer.Trace(EnumTraceLevel.INF, traceGroup, "Service::Main", "Registration succeeded")
				Else
					m_opcServer.Trace(EnumTraceLevel.INF, traceGroup, "Service::Main", "Registration failed")
				End If

				' no matter what close the application if
				' processCommandLine returned something different of S_OK        
				m_opcServer.Terminate()
				m_opcServer = Nothing
				Return
			End If
			System.ServiceProcess.ServiceBase.Run(ServicesToRun)
		End Sub

		''' <summary>
		''' Clean up any resources being used.
		''' </summary>
		Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
			MyBase.Dispose(disposing)
		End Sub

		''' <summary>
		''' Set things in motion so your service can do its work.
		''' </summary>
		Protected Overloads Overrides Sub OnStart(ByVal args As String())
			'  start the OPC server's I/O internal mechanism 
			If ResultCode.SUCCEEDED(m_opcServer.Start()) Then
				'  build the namespace
				m_opcServer.BuildAddressSpace()

				'  declare the namespaces built and the server ready for clients to connect
				m_opcServer.Ready()

				StartSimulationThread()
			End If
		End Sub

		''' <summary>
		''' Stop this service.
		''' </summary>
		Protected Overloads Overrides Sub OnStop()

			Try
				'  terminate the simulation
				SimulationEnd = True
				Mutex.WaitOne()
				Mutex.ReleaseMutex()
				m_opcServer.[Stop]()
				m_opcServer.Terminate()
				m_opcServer = Nothing
			Catch exc As Exception
				Dim fs As New FileStream("C:\\Service_TraceData.txt", FileMode.OpenOrCreate, FileAccess.Write)
				Dim streamWriter As New StreamWriter(fs)
				streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
				streamWriter.WriteLine()
				streamWriter.WriteLine(exc.ToString())
				streamWriter.Flush()
				streamWriter.Close()
			End Try
		End Sub
	End Class
End Namespace

