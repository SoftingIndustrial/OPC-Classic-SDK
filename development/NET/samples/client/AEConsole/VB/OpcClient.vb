Imports System
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace AEConsole
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
			' disconnect all the connected objects
			If m_aeSubscription.CurrentState <> EnumObjectState.DISCONNECTED Then
				m_aeSubscription.Disconnect(New ExecutionOptions)
			End If

			If m_aeSession.CurrentState <> EnumObjectState.DISCONNECTED Then
				m_aeSession.Disconnect(New ExecutionOptions)
			End If

			' remove subscription from session
			m_aeSession.RemoveAeSubscription(m_aeSubscription)

			' remove session from application
			GetApplication().RemoveAeSession(m_aeSession)

			' terminate the application
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
			m_executionOptions = New ExecutionOptions
			m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS
			m_executionOptions.ExecutionContext = Convert.ToUInt32(0)

			Try

				m_aeSession = New MyAeSession("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}")
				m_aeSubscription = New MyAeSubscription(m_aeSession)

				connectResult = m_aeSession.Connect(True, False, New ExecutionOptions)

				'define the event areas that will be used to filter events
				'TODO replace the array below with your own areas
				' NOTE: Areas excluded from this filter will not send events/conditions to the AE client.
				'       If no filter is set, all areas will fire events.
				Dim areas As String() = New String() {"computer.mouse", "computer.clock"}

				'set the previously defined areas for filtering
				m_aeSubscription.FilterAreas = areas

				'define the event sources that will be used to filter events
				'TODO replace the array below with your own sources
				' NOTE: Sources excluded from this filter will not send events/conditions to the AE client.
				'       If no filter is set, all sources will fire events.
				Dim sources As String() = New String() {"computer.mouse.right button", "computer.mouse.middle button", "computer.clock.timer", "computer.clock.time slot 1", "computer.clock.time slot 2"}

				'set the previously defined sources for filtering
				m_aeSubscription.FilterSources = sources

				'define the categories that will be used to filter events ("time tick" category is used)
				'TODO replace the array below with your own category ids
				Dim categoryIds As UInt32() = New UInt32() {Convert.ToUInt32(1), Convert.ToUInt32(2), Convert.ToUInt32(3)}

				'set the previously defines categoryIds for filtering
				' NOTE: Category IDs excluded from this filter will not send events/conditions to the AE client.
				'       If no filter is set, all categories will fire events.
				m_aeSubscription.FilterCategories = categoryIds

				' NOTE: Event/conditions property IDs not included in this filter will not be displayed.
				'       If no property filter is set, NO PROPERTIES WILL BE SENT WITH EACH EVENT/CONDITION (except for standard AE event/condition properties).
				Dim returnedAttributes As AeReturnedAttributes() = New AeReturnedAttributes(2) {}

				' category ID: 1 (time tick)
				' attribute IDs: 1 (second)
				Dim attributeIds As UInt32() = New UInt32(0) {}
				attributeIds(0) = Convert.ToUInt32(1)
				returnedAttributes(0) = New AeReturnedAttributes
				returnedAttributes(0).AttributeIds = attributeIds
				returnedAttributes(0).CategoryId = Convert.ToUInt32(1)

				' category ID: 2 (time slot)
				' attribute IDs: 2 (second), 3 (start second), 4 (end second)
				attributeIds = New UInt32(2) {}
				attributeIds(0) = Convert.ToUInt32(2)
				attributeIds(1) = Convert.ToUInt32(3)
				attributeIds(2) = Convert.ToUInt32(4)
				returnedAttributes(1) = New AeReturnedAttributes
				returnedAttributes(1).AttributeIds = attributeIds
				returnedAttributes(1).CategoryId = Convert.ToUInt32(2)

				' category ID: 3 (mouse click)
				' attribute IDs: 5 (x position), 6 (y position)
				attributeIds = New UInt32(1) {}
				attributeIds(0) = Convert.ToUInt32(5)
				attributeIds(1) = Convert.ToUInt32(6)
				returnedAttributes(2) = New AeReturnedAttributes
				returnedAttributes(2).AttributeIds = attributeIds
				returnedAttributes(2).CategoryId = Convert.ToUInt32(3)

				m_aeSubscription.ReturnedAttributes = returnedAttributes

			Catch exc As Exception
				GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::InitializeAeObjects", exc.ToString())
			End Try

			Return connectResult
		End Function

		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub

		Public Sub activateObjectsAsync()
			System.Console.WriteLine()
			' activate the session asynchronously
			m_aeSession.Connect(True, True, m_executionOptions)
			' increment the execution context used to identify the callback that comes as response
			m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
		End Sub

		Public Sub activateObjectsSync()
			System.Console.WriteLine()
			' activate the session synchronously
			m_aeSession.Connect(True, True, New ExecutionOptions)
		End Sub

		Public Sub connectObjectsAsync()
			System.Console.WriteLine()
			' connect the session asynchronously
			m_aeSession.Connect(True, False, m_executionOptions)
			' increment the execution context used to identify the callback that comes as response
			m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
		End Sub

		Public Sub connectObjectsSync()
			System.Console.WriteLine()
			' connect the session synchronously
			m_aeSession.Connect(True, False, New ExecutionOptions)
		End Sub

		Public Sub disconnectObjectsAsync()
			System.Console.WriteLine()
			' disconnect the session asynchronously
			m_aeSession.Disconnect(m_executionOptions)
			' increment the execution context used to identify the callback that comes as response
			m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
		End Sub

		Public Sub disconnectObjectsSync()
			' disconnect the session synchronously
			System.Console.WriteLine()
			m_aeSession.Disconnect(New ExecutionOptions)
		End Sub

		Public Sub getServerStatusAsync()
			Dim newServerStatus As ServerStatus = Nothing
			' get the session status asynchronously
			Dim statusRes As Integer = m_aeSession.GetStatus(newServerStatus, m_executionOptions)
			' increment the execution context used to identify the callback that comes as response
			m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
		End Sub

		Public Sub getServerStatusSync()
			Dim serverStatus As ServerStatus = Nothing
			' get the session status synchronously
			If ResultCode.SUCCEEDED(m_aeSession.GetStatus(serverStatus, New ExecutionOptions)) Then

				System.Console.WriteLine(" Server Status")
				System.Console.WriteLine("  Vendor info: " + serverStatus.VendorInfo)
				System.Console.WriteLine("  Product version: " + serverStatus.ProductVersion)
				System.Console.WriteLine("  State: " + serverStatus.State.ToString())
				System.Console.WriteLine("  Start time: " + serverStatus.StartTime.ToString())
				System.Console.WriteLine("  Last update time: " + serverStatus.LastUpdateTime.ToString())
				System.Console.WriteLine("  Current time: " + serverStatus.CurrentTime.ToString())
				System.Console.WriteLine("  GroupCount: " + serverStatus.GroupCount.ToString())
				System.Console.WriteLine("  Bandwidth: " + serverStatus.Bandwidth.ToString())
				Dim i As Integer = 0
				While i < serverStatus.SupportedLcIds.Length

					System.Console.WriteLine("  Supported LCID: " + serverStatus.SupportedLcIds(i))
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
				System.Console.WriteLine("  Status info: " + serverStatus.StatusInfo)
			Else
				System.Console.WriteLine(" Get Status failed ")
			End If
		End Sub

		Public Sub activateConnectionMonitor()
			' activate the monitor that watches the connection status
			If ResultCode.SUCCEEDED(m_aeSession.ActivateConnectionMonitor(True, Convert.ToUInt32(5000), Convert.ToUInt32(0), Convert.ToUInt32(10000), Convert.ToUInt32(300000))) Then
				System.Console.WriteLine()
				System.Console.WriteLine("Activated connection monitor")
				System.Console.WriteLine()
			Else
				System.Console.WriteLine("Activate connection monitor failed")
			End If
		End Sub

		Public Sub deactivateConnectionMonitor()
			' deactivate the monitor that watches the connection status
			If ResultCode.SUCCEEDED(m_aeSession.ActivateConnectionMonitor(False, Convert.ToUInt32(0), Convert.ToUInt32(0), Convert.ToUInt32(0), Convert.ToUInt32(0))) Then
				System.Console.WriteLine()
				System.Console.WriteLine("Deactivated connection monitor")
				System.Console.WriteLine()
			Else
				System.Console.WriteLine("Deactivate connection monitor failed")
			End If
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

