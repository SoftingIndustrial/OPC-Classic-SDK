Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DABrowse
	Public Class OpcClient

#Region "Constructor"
		'-----------------

		Public Sub New()
		End Sub

		'--
#End Region

#Region "Private Members"
		'---------------------

#Region "Private Attributes"
		'------------------------

		Private m_daSession As MyDaSession = Nothing
		Private m_executionOptions As ExecutionOptions = Nothing
		'--
#End Region


		'--
#End Region

#Region "Public Methods"
		'---------------------

		Public Function GetApplication() As Application
			Return Application.Instance
		End Function

		Public Function Initialize() As Integer

			Dim traceGroupClient As EnumTraceGroup
			Dim traceGroupAll As EnumTraceGroup
			traceGroupClient = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.CLIENT)
			traceGroupAll = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL)


			Dim result As Integer = EnumResultCode.S_OK
			GetApplication().VersionOtb = 447
			'  enable toolkit traces
			GetApplication().EnableTracing(traceGroupAll, traceGroupAll, traceGroupClient, traceGroupClient, "Trace.txt", Convert.ToUInt32(1000000), _
			 Convert.ToUInt32(0))
			'  TODO - binary license activation
			'  Fill in your binary license activation keys here
			'
			'  NOTE: you can activate one or all of the features at the same time

			'  activate the COM-DA Client feature
			'  result = Application.Instance.Activate(EnumFeature.DA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX")
			If Not ResultCode.SUCCEEDED(result) Then
				Return result
			End If

			'  activate the XML-DA Client Feature
			'  result = Application.Instance.Activate(EnumFeature.XMLDA_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX")
			If Not ResultCode.SUCCEEDED(result) Then
				Return result
			End If
			'  END TODO - binary license activation

			'  proceed with the OPC Toolkit core initialization
			result = GetApplication().Initialize()

			Return result
		End Function

		Public Function ProcessCommandLine(ByVal commandLine As String) As Integer
			'  forward the command line arguments to the OPC Toolkit core internals
			Return Application.Instance.ProcessCommandLine(commandLine)
		End Function


		Public Sub Terminate()
			If m_daSession.CurrentState <> EnumObjectState.DISCONNECTED Then
				m_daSession.Disconnect(New ExecutionOptions)
			End If
			GetApplication().RemoveDaSession(m_daSession)

			GetApplication().Terminate()
			m_daSession = Nothing
			m_executionOptions = Nothing
		End Sub


		Public Function InitializeDaObjects() As Integer
			Dim connectResult As Integer = EnumResultCode.E_FAIL
			m_executionOptions = New ExecutionOptions

			Try

				'  TODO add your server URL here
				'  this is the server url for Softing OPCToolbox Demo Server
				'  first choice is COM-DA 
				Dim url As String = "opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"

				'uncomment if you need an XML-DA access
				'Dim url As String = "http://localhost:8079/OPC/DA"

				m_daSession = New MyDaSession(url)

				connectResult = m_daSession.Connect(True, False, m_executionOptions)
			Catch exc As Exception
				Dim traceGroupUser As EnumTraceGroup
				traceGroupUser = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER)
				GetApplication().Trace(EnumTraceLevel.ERR, traceGroupUser, "OpcClient::InitializaDaObjects", exc.ToString())
			End Try

			Return connectResult
		End Function


		Public Function GetSession() As MyDaSession
			Return m_daSession
		End Function

		Public Function GetExecutionOptions() As ExecutionOptions
			Return m_executionOptions
		End Function


		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub

		'--
#End Region

#Region "Public Properties"
		'-----------------------

		Public Property ServiceName() As String
			Get
				Return Application.Instance.ServiceName
			End Get
			Set(ByVal Value As String)
				Application.Instance.ServiceName = Value
			End Set
		End Property

		'--
#End Region

	End Class

End Namespace

