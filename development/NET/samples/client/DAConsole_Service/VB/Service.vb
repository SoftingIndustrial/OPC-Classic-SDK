Imports System
Imports System.IO
Imports Softing.OPCToolbox

Namespace DaConsole_Service
	Public Class OpcService
		Inherits System.ServiceProcess.ServiceBase

#Region "Private Attributes"
		'-------------------------

		Private Shared m_opcClient As OpcClient = Nothing
		' The following constant holds the name and the description of the Windows NT service
		' the OPC application runs under.
		'    TODO : change your service name and description here
		Private Shared defaultServiceName As String = "DaConsole_Service OpcService"
		Private Shared defaultServiceDescription As String = "Softing OPC Toolkit - OPC DA Client. " & Environment.NewLine & Environment.NewLine & "Sample name: ""VB DaConsole_Service"""
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
		'--------------------

		Public Sub CreateOpcClient()
			If m_opcClient Is Nothing Then
				m_opcClient = New OpcClient()
			End If
		End Sub
		'--
#End Region

		Public Sub New()
		End Sub

		' The main entry point for the process
		Shared Sub Main()

			Dim result As Integer = EnumResultCode.S_OK
			Dim commandLineProcessed As Boolean = False
			Dim commandline As String = Environment.CommandLine

			Dim ServicesToRun As System.ServiceProcess.ServiceBase()
			Dim opcService As New OpcService
			ServicesToRun = New System.ServiceProcess.ServiceBase() {opcService}

			Dim traceGroupUser1 As EnumTraceGroup
			traceGroupUser1 = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1)


			opcService.CreateOpcClient()
			m_opcClient.ServiceName = defaultServiceName
			m_opcClient.ServiceDescription = defaultServiceDescription
			'  initialize the client instance
			If Not ResultCode.SUCCEEDED(m_opcClient.Initialize()) Then
				m_opcClient = Nothing
				Return
			End If

			If Not commandLineProcessed Then
				result = m_opcClient.ProcessCommandLine(commandline)
				commandLineProcessed = True
				If result <> EnumResultCode.S_OK Then
					If result = EnumResultCode.S_FALSE Then
						'registration operation succesful
						m_opcClient.Trace(EnumTraceLevel.INF, traceGroupUser1, "Service::Main", "Registration succeeded")
					Else
						m_opcClient.Trace(EnumTraceLevel.INF, traceGroupUser1, "Service::Main", "Registration failed")
					End If

					'  no matter what close the application if
					'processCommandLine returned something different of S_OK        
					m_opcClient.Terminate()
					m_opcClient = Nothing

					Return
				End If
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
			Dim result As Integer = EnumResultCode.S_OK
			'  initialize the DA client simulation
			result = result Or m_opcClient.InitializeDaObjects()

		End Sub

		''' <summary>
		''' Stop this service.
		''' </summary>
		Protected Overloads Overrides Sub OnStop()

			Try
				m_opcClient.Terminate()
				m_opcClient = Nothing
			Catch exc As Exception
				Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
				Dim streamWriter As New StreamWriter(fs)
				streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
				streamWriter.WriteLine()
				streamWriter.WriteLine(exc.Message.ToString() + exc.Source.ToString() + exc.StackTrace.ToString() + exc.TargetSite.ToString())
				streamWriter.Flush()
				streamWriter.Close()
			End Try
		End Sub
	End Class
End Namespace

