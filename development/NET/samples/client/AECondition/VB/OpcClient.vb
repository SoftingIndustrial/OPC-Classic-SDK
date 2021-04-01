Imports System
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace AECondition
	Public Class OpcClient

#Region "Fields"
		Dim m_traceGroupUser As EnumTraceGroup
#End Region

#Region "Constructor"
		'-----------------

		Public Sub New()
			m_traceGroupUser = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER)
		End Sub

		'--
#End Region

#Region "Private Members"
		'---------------------

#Region "Private Attributes"
		'------------------------

		Private m_aeSession As MyAeSession = Nothing
		Private m_aeSubscription As MyAeSubscription = Nothing
		Private m_executionOptions As ExecutionOptions = Nothing
		Private m_opcForm As OpcForm = Nothing
		'--
#End Region

#Region "Private Methods"
		'---------------------

		Private Function GetStateToString(ByVal state As EnumConditionState) As String
			Dim stateToString As String = String.Empty

			If (state And EnumConditionState.ACTIVE) = EnumConditionState.ACTIVE Then
				stateToString &= " ACT"
			End If
			If (state And EnumConditionState.ENABLED) = EnumConditionState.ENABLED Then
				stateToString &= " ENA"
			End If
			If (state And EnumConditionState.ACKED) = EnumConditionState.ACKED Then
				stateToString &= " ACK"
			End If
			If state = 0 Then
				stateToString &= " DIS"
			End If
			Return stateToString
		End Function

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

			'  activate the COM-AE Client Feature
			'  result = Application.Instance.Activate(EnumFeature.AE_CLIENT, "XXXX-XXXX-XXXX-XXXX-XXXX")
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
			If m_aeSubscription.CurrentState <> EnumObjectState.DISCONNECTED Then
				m_aeSubscription.Disconnect(m_executionOptions)
			End If

			If m_aeSession.CurrentState <> EnumObjectState.DISCONNECTED Then
				m_aeSession.Disconnect(m_executionOptions)
			End If

			m_aeSession.RemoveAeSubscription(m_aeSubscription)
			GetApplication().RemoveAeSession(m_aeSession)

			GetApplication().Terminate()
			m_aeSession = Nothing
			m_aeSubscription = Nothing
			m_executionOptions = Nothing
		End Sub


		Public Function GetConditionState() As String
			If m_aeSession Is Nothing Then
				Return ""
			End If

			Dim message As String = [String].Empty

			Try
				Dim conditionStateToString As String = [String].Empty
				Dim conditionState As AeConditionState = Nothing

				Dim result As Integer = m_aeSession.GetAeConditionState("computer.clock.time slot 1", "between", Nothing, conditionState, Nothing)

				If ResultCode.SUCCEEDED(result) Then

					message = "The condition state is: " & Chr(10) & ""
					message &= " Source Path: computer.clock.time slot 1 " & Chr(10) & ""
					message &= " Condition Name: between " & Chr(10) & ""
					message &= " State: " + GetStateToString(conditionState.State) + "" & Chr(10) & ""
					message &= " Quality: " + conditionState.Quality.ToString() + "" & Chr(10) & ""
					message &= " Active Time: " + conditionState.ConditionActiveTime.ToString() + "" & Chr(10) & ""
					message &= " Inactive Time: " + conditionState.ConditionInactiveTime.ToString() + "" & Chr(10) & ""
					message &= " AcknowledgeTime: " + conditionState.ConditionAckTime.ToString() + "" & Chr(10) & ""
					message &= " AcknowledgerComment: " + conditionState.AcknowledgerComment + "" & Chr(10) & ""
					message &= " AcknowledgerID: " + conditionState.AcknowledgerId + "" & Chr(10) & ""
					message &= " Active subcondition time: " + conditionState.SubConditionActiveTime.ToString() + "" & Chr(10) & ""
					message &= " Active subcondition name: " + conditionState.ActiveSubConditionName + "" & Chr(10) & ""
					message &= " Active subcondition definition: " + conditionState.ActiveSubConditionDefinition + "" & Chr(10) & ""
					message &= " Active subcondition description: " + conditionState.ActiveSubConditionDescription + "" & Chr(10) & ""
					message &= " Active subcondition severity: " + conditionState.ActiveSubConditionSeverity.ToString() + "" & Chr(10) & ""
					message &= " Number of subconditions: " + conditionState.SubConditionsNames.Length.ToString() + "" & Chr(10) & ""
					Dim i As Integer = 0
					While i < conditionState.SubConditionsNames.Length
						message &= "  Subcondition name: " + conditionState.SubConditionsNames(i) + "" & Chr(10) & ""
						message &= "  Subcondition definition: " + conditionState.SubConditionsDefinitions(i) + "" & Chr(10) & ""
						message &= "  Subcondition description: " + conditionState.SubConditionsDescriptions(i) + "" & Chr(10) & ""
						message &= "  Subcondition severity: " + conditionState.SubConditionsSeverities(i).ToString() + "" & Chr(10) & ""
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				Else
					message = "Get condition state failed!" & Chr(10) & ""
				End If
			Catch exc As Exception
				GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::GetConditionState", exc.ToString())
			End Try
			Return message
		End Function

		Public Function InitializeAeObjects() As Integer
			Dim connectResult As Integer = EnumResultCode.E_FAIL
			Dim result As Integer = EnumResultCode.S_OK

			m_executionOptions = New ExecutionOptions

			Try

				m_aeSession = New MyAeSession("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}")
				m_aeSubscription = New MyAeSubscription(m_aeSession)

				connectResult = m_aeSession.Connect(True, True, m_executionOptions)

				result = m_aeSubscription.RefreshAeConditions(m_executionOptions)

				If Not ResultCode.SUCCEEDED(result) Then
					Dim errorText As String = [String].Empty
					m_aeSession.GetErrorString(result, errorText, m_executionOptions)
					System.Diagnostics.Debug.WriteLine(errorText)
				End If
			Catch exc As Exception
				GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::InitializeAeObjects", exc.ToString())
			End Try

			Return connectResult
		End Function

		Public Sub SetForm(ByVal form As OpcForm)
			m_opcForm = form
			m_aeSession.SetForm(m_opcForm)
			m_aeSubscription.SetForm(m_opcForm)
		End Sub

		Public Function GetExecutionOptions() As ExecutionOptions
			Return m_executionOptions
		End Function

		Public Function GetSession() As MyAeSession
			Return m_aeSession
		End Function

		Public Function GetSubscription() As MyAeSubscription
			Return m_aeSubscription
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

