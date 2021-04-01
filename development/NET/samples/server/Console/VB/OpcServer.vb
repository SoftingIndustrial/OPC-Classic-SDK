Imports System
Imports System.Collections
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace Console
	Public Class OpcServer
#Region "Constants"
		'----------------
		Private Const CATEGORY_ID_SIMULATION As Integer = 1
		Private Const ATTRIBUTE_ID_SIMULATION As Integer = 1
		'--
#End Region

#Region "Fields"
		Dim m_qualityGood As EnumQuality
		Dim m_traceGroupUser1 As EnumTraceGroup
		Dim m_traceGroupOpcServer As EnumTraceGroup
		Dim m_traceGroupServer As EnumTraceGroup
		Dim m_traceGroupAll As EnumTraceGroup
#End Region

#Region "Constructor"
		'-----------------

		Public Sub New()
			m_qualityGood = [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD)
			m_traceGroupUser1 = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1)
			m_traceGroupOpcServer = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.OPCSERVER)
			m_traceGroupOpcServer = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER)
			m_traceGroupAll = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL)
		End Sub

		'--
#End Region

#Region "Private Attributes"
		'--------------------------

		Private Shared _timeTick As AeCategory = Nothing
		Private Shared _timeSlot As AeCategory = Nothing
		Private condition1 As AeCondition = Nothing
		Private condition2 As AeCondition = Nothing
		Private eventAttributes1 As ArrayList = Nothing
		Private eventAttributes2 As ArrayList = Nothing

		'--
#End Region

#Region "Protected Attributes"
		'--------------------------

		Protected m_shutdownRequest As ShutdownHandler = Nothing
		Protected m_daSimulationElement As MyDaAddressSpaceElement = Nothing
		Protected m_random As New Random(1)
		'--
#End Region

#Region "Public Attributes"
		'--------------------------

		Public Shared g_angle As Integer = 0
		Public Shared g_sin As MyDaAddressSpaceElement
		Public Shared g_rand1s As MyDaAddressSpaceElement
		Public Shared g_rand100ms As MyDaAddressSpaceElement

		'--
#End Region

#Region "Public Constants"
		'--------------------------

		Public Const COND_NAME_BETWEEN_SINGLE As String = "between [10,50]"
		Public Const COND_NAME_BETWEEN_MULTIPLE As String = "between"

		Public Const CAT_ID_TIME_TICK As Integer = 1
		Public Const CAT_ID_TIME_SLOT As Integer = 2

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

#Region "Public Methods"
		'-----------------------

		Public Shared Function Shutdown() As Integer
			Return EnumResultCode.S_OK
		End Function


		Public Function Initialize() As Integer
			Try
				Application.Instance.VersionOtb = 447
				Application.Instance.AppType = EnumApplicationType.EXECUTABLE
				Application.Instance.ClsidDa = "{D7555520-7ACC-4F71-8D0D-28DEC0ACC7DC}"
				Application.Instance.ProgIdDa = "Softing.OPC.Toolkit.NET.VB.Sample.Console.DA.1"
				Application.Instance.VerIndProgIdDa = "Softing.OPC.Toolkit.NET.VB.Sample.Console.DA"

				Application.Instance.ClsidAe = "{680B5EF2-F786-4C78-9FC9-EB85912D7305}"
				Application.Instance.ProgIdAe = "Softing.OPC.Toolkit.NET.VB.Sample.Console.AE.1"
				Application.Instance.VerIndProgIdAe = "Softing.OPC.Toolkit.NET.VB.Sample.Console.AE"

				Application.Instance.Description = "Softing OPC Toolkit .NET VB Console Sample Server"
				Application.Instance.ServiceName = String.Empty
				Application.Instance.IpPortHTTP = Convert.ToUInt16(8079)
				Application.Instance.UrlDa = "/OPC/DA"
				Application.Instance.MajorVersion = 4
				Application.Instance.MinorVersion = 47
				Application.Instance.PatchVersion = 1
				Application.Instance.BuildNumber = 0
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH"
				Application.Instance.MinUpdateRateDa = 100
				Application.Instance.ClientCheckPeriod = 30000
				Application.Instance.AddressSpaceDelimiter = "/"c
				Application.Instance.PropertyDelimiter = "#"c
				Application.Instance.SupportDisableConditions = True
				Application.Instance.WebRootDirectory = "www"
				Application.Instance.WebRootFile = String.Empty
				Application.Instance.WebAdministratorPassword = "ad"
				Application.Instance.WebOperatorPassword = "op"
				Application.Instance.EnableTracing(m_traceGroupAll, m_traceGroupAll, m_traceGroupServer, m_traceGroupServer, "Trace.txt", Convert.ToUInt32(1000000), _
				 Convert.ToUInt32(0))

				AddHandler Application.Instance.ShutdownRequest, AddressOf Shutdown

			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, m_traceGroupUser1, "OpcServer::Initialize", exc.ToString())

				Return EnumResultCode.E_FAIL
			End Try
			Return EnumResultCode.S_OK
		End Function

		Public Function Start() As Integer
			Return Application.Instance.Start()
		End Function

		Public Function [Stop]() As Integer
			Return Application.Instance.[Stop]()
		End Function

		Public Function Ready() As Integer
			Return Application.Instance.Ready()
		End Function

		Public Function Terminate() As Integer
			Return Application.Instance.Terminate()
		End Function

		Public Function Prepare(ByVal aMyCreator As MyCreator) As Integer
			Dim result As Integer = EnumResultCode.S_OK
			'  TODO - binary license activation
			'  Fill in your binary license activation keys here
			'
			'  NOTE: you can activate one or all of the features at the same time

			'  activate the COM-DA Server feature
			'  result = Application.Instance.Activate(EnumFeature.DA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX")
			If Not ResultCode.SUCCEEDED(result) Then
				Return result
			End If

			'  activate the XML-DA Server Feature
			'  result = Application.Instance.Activate(EnumFeature.XMLDA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX")
			If Not ResultCode.SUCCEEDED(result) Then
				Return result
			End If

			'  activate the COM-AE Server Feature
			'  result = Application.Instance.Activate(EnumFeature.AE_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX")
			If Not ResultCode.SUCCEEDED(result) Then
				Return result
			End If
			'  END TODO - binary license activation

			result = Application.Instance.Initialize(aMyCreator)

			Return result
		End Function

		Public Function ProcessCommandLine(ByVal commandLine As String) As Integer
			Return Application.Instance.ProcessCommandLine(commandLine)
		End Function

		Public Function BuildAddressSpace() As Integer
			Try
				Dim tag As MyDaAddressSpaceElement, node As MyDaAddressSpaceElement

				tag = New MyDaAddressSpaceElement
				tag.Name = "accept"
				tag.Type = MyDaAddressSpaceElement.TYPE_ACCEPT
				tag.AccessRights = EnumAccessRights.READWRITEABLE
				tag.Datatype = GetType(Int32)
				tag.IoMode = EnumIoMode.POLL
				Application.Instance.DaAddressSpaceRoot.AddChild(tag)

				node = New MyDaAddressSpaceElement
				node.Name = "maths"
				node.Type = MyDaAddressSpaceElement.TYPE_NODEMATH
				node.IoMode = EnumIoMode.NONE
				node.HasChildren = True
				Application.Instance.DaAddressSpaceRoot.AddChild(node)

				g_sin = New MyDaAddressSpaceElement
				g_sin.Name = "sin"
				g_sin.Type = MyDaAddressSpaceElement.TYPE_SINE
				g_sin.AccessRights = EnumAccessRights.READABLE
				g_sin.Datatype = GetType(Double)
				g_sin.IoMode = EnumIoMode.REPORT
				node.AddChild(g_sin)

				Dim euType As New DaConstantProperty(7, "euType", "7", New ValueQT(Convert.ToUInt32(EnumEUType.ANALOG), m_qualityGood, DateTime.Now))
				euType.Datatype = GetType(Int32)
				g_sin.AddProperty(euType)

				Dim euHigh As New DaConstantProperty(102, "High EU", "102", New ValueQT(Convert.ToDouble(1), m_qualityGood, DateTime.Now))
				euHigh.Datatype = GetType(Double)
				g_sin.AddProperty(euHigh)

				Dim euLow As New DaConstantProperty(103, "Low EU", "103", New ValueQT(Convert.ToDouble(-1), m_qualityGood, DateTime.Now))
				euLow.Datatype = GetType(Double)
				g_sin.AddProperty(euLow)

				Dim [property] As New DaProperty
				[property].Id = 6020
				[property].Name = "Angle"
				[property].Description = "Angle Description"
				[property].ItemId = [property].Name
				[property].Datatype = GetType(Short)
				[property].AccessRights = EnumAccessRights.READWRITEABLE
				g_sin.AddProperty([property])

				node = New MyDaAddressSpaceElement
				node.Name = "random"
				node.Type = MyDaAddressSpaceElement.TYPE_NODERANDOM
				node.IoMode = EnumIoMode.NONE
				node.HasChildren = True
				Application.Instance.DaAddressSpaceRoot.AddChild(node)

				g_rand100ms = New MyDaAddressSpaceElement
				g_rand100ms.Name = "100ms"
				g_rand100ms.Type = MyDaAddressSpaceElement.TYPE_RANDOM_100MS
				g_rand100ms.AccessRights = EnumAccessRights.READABLE
				g_rand100ms.Datatype = GetType(Int32)
				g_rand100ms.IoMode = EnumIoMode.REPORT
				node.AddChild(g_rand100ms)

				g_rand1s = New MyDaAddressSpaceElement
				g_rand1s.Name = "1s"
				g_rand1s.Type = MyDaAddressSpaceElement.TYPE_RANDOM_1S
				g_rand1s.AccessRights = EnumAccessRights.READABLE
				g_rand1s.Datatype = GetType(Int32)
				g_rand1s.IoMode = EnumIoMode.REPORT
				node.AddChild(g_rand1s)

				'  AE
				Dim computer As New MyAeAddressSpaceElement
				computer.Name = "computer"
				computer.HasChildren = True
				Application.Instance.AeAddressSpaceRoot.AddChild(computer)

				Dim clock As New MyAeAddressSpaceElement
				clock.Name = "clock"
				clock.HasChildren = True
				computer.AddChild(clock)

				Dim timer As New MyAeAddressSpaceElement
				timer.Name = "timer"
				timer.HasChildren = False
				clock.AddChild(timer)

				timer = New MyAeAddressSpaceElement
				timer.Name = "time slot 1"
				timer.HasChildren = False
				clock.AddChild(timer)

			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, m_traceGroupUser1, "OpcServer:BuildAddressSpace", exc.ToString())
				Return EnumResultCode.E_FAIL
			End Try

			Return EnumResultCode.S_OK
		End Function


		Public Function BuildEventCategories() As Integer
			Dim result As Integer = 0
			Try
				_timeTick = New AeCategory(EnumEventType.SIMPLE, Convert.ToUInt32(CAT_ID_TIME_TICK), "time tick")
				result = Application.Instance.AddAeCategory(_timeTick)

				If ResultCode.SUCCEEDED(result) Then
					_timeTick.AddAttribute(Convert.ToUInt32(1), "hour", GetType(System.UInt16))
					_timeTick.AddAttribute(Convert.ToUInt32(2), "minute", GetType(System.UInt16))
					_timeTick.AddAttribute(Convert.ToUInt32(3), "second", GetType(System.UInt16))
				Else
					Application.Instance.Trace(EnumTraceLevel.WRN, m_traceGroupOpcServer, "Console.BuildEventCategories", "AddAeCategory has failed for <timeTick>")
				End If

				_timeSlot = New AeCategory(EnumEventType.CONDITION, Convert.ToUInt32(CAT_ID_TIME_SLOT), "time slot 1")
				result = Application.Instance.AddAeCategory(_timeSlot)
				If ResultCode.SUCCEEDED(result) Then
					_timeSlot.AddAttribute(Convert.ToUInt32(1), "second", GetType(System.UInt16))
					_timeSlot.AddConditionDefinition(COND_NAME_BETWEEN_SINGLE)
					_timeSlot.AddConditionDefinition(COND_NAME_BETWEEN_MULTIPLE)
					_timeSlot.AddSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, "[10,30[")
					_timeSlot.AddSubConditionDefinition(COND_NAME_BETWEEN_MULTIPLE, "[30,50]")
				Else
					Application.Instance.Trace(EnumTraceLevel.WRN, m_traceGroupOpcServer, "Console.BuildEventCategories", "AddAeCategory has failed for <time slot 1>")
				End If
			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, m_traceGroupUser1, "OpcServer::BuildEventCategories", exc.ToString())
				Return EnumResultCode.E_FAIL
			End Try

			Return EnumResultCode.S_OK

		End Function

		Public Function InitializeConditions() As Integer
			Try
				condition1 = New AeCondition
				condition2 = New AeCondition

				eventAttributes1 = New ArrayList
				eventAttributes1.Add(1)

				eventAttributes2 = New ArrayList
				eventAttributes2.Add(2)

				condition1.Category = _timeSlot
				condition1.Definition = _timeSlot.GetConditionDefinition(COND_NAME_BETWEEN_MULTIPLE)
				condition1.Name = COND_NAME_BETWEEN_MULTIPLE

				condition1.SourcePath = "computer/clock/time slot 1"
				condition1.Quality = m_qualityGood
				condition1.EventAttributes = eventAttributes1
				condition1.UserData = Convert.ToUInt32(1)

				condition2.Category = _timeSlot
				condition2.Name = COND_NAME_BETWEEN_SINGLE
				condition2.Definition = _timeSlot.GetConditionDefinition(COND_NAME_BETWEEN_SINGLE)
				condition2.SourcePath = "computer/clock/time slot 1"
				condition2.Quality = m_qualityGood
				condition2.EventAttributes = eventAttributes2
				condition2.UserData = Convert.ToUInt32(2)

			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, m_traceGroupUser1, "OpcServer:initializeConditions", exc.ToString())
				Return EnumResultCode.E_FAIL
			End Try

			Return EnumResultCode.S_OK
		End Function


		Public Sub FireSimulationEvents()
			'simple events
			Dim events As New ArrayList
			Dim simulationEvent As New AeEvent(EnumEventType.SIMPLE, "SimulationElement", Convert.ToUInt32(CATEGORY_ID_SIMULATION))

			simulationEvent.Severity = Convert.ToUInt32(10)
			simulationEvent.ActorId = "OpcUser"
			simulationEvent.Message = "Simulation event generated"
			events.Add(simulationEvent)

			Application.Instance.FireEvents(events)
		End Sub

		Public Sub ChangeSimulationValues()
			If Not m_daSimulationElement Is Nothing Then
				m_daSimulationElement.ValueChanged(New ValueQT(m_random.[Next](5000), m_qualityGood, DateTime.Now))

			End If
		End Sub

		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub

		Public Sub CheckValuesChanged(ByRef r_dt As DateTime)
			Dim dateTimeNow As DateTime = DateTime.UtcNow
			Dim rand As New Random

			Dim bSecChanged As Boolean = r_dt.Second <> dateTimeNow.Second
			If bSecChanged Then
				r_dt = dateTimeNow
			End If

			Dim changedElements As New ArrayList
			Dim changedValues As New ArrayList
			Dim val As ValueQT = Nothing
			val = New ValueQT(rand.[Next](), m_qualityGood, DateTime.Now)
			changedElements.Add(g_rand100ms)
			changedValues.Add(val)

			If bSecChanged Then
				val = New ValueQT(rand.[Next](), m_qualityGood, DateTime.Now)
				changedElements.Add(g_rand1s)
				changedValues.Add(val)

				g_angle = ((System.Threading.Interlocked.Increment(g_angle)) Mod 360)
				val = New ValueQT(Math.Sin(g_angle / 180 * 3.14), m_qualityGood, DateTime.Now)
				changedElements.Add(g_sin)
				changedValues.Add(val)
			End If

			Application.Instance.DaAddressSpaceRoot.ValuesChanged(changedElements, changedValues)

		End Sub

		Public Sub AtSecond10(ByRef r_conditionList As ArrayList)
			condition1.Severity = Convert.ToUInt32(500)
			condition1.Message = "between [10,30["
			condition1.AckRequired = True
			condition1.ActiveSubConditionName = "[10,30["
			condition1.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.SUBCONDITION Or EnumConditionChange.ATTRIBUTE)

			condition1.StateChange = EnumConditionState.ACTIVE

			condition2.Severity = Convert.ToUInt32(510)
			condition2.Message = "between [10,50]"
			condition2.AckRequired = True
			condition2.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.ATTRIBUTE)
			condition2.StateChange = EnumConditionState.ACTIVE

			r_conditionList.Add(condition1)
			r_conditionList.Add(condition2)

		End Sub

		Public Sub AtSecond30(ByRef r_conditionList As ArrayList)
			condition1.Severity = Convert.ToUInt32(540)
			condition1.Message = "between [30,50]"
			condition1.AckRequired = True
			condition1.ActiveSubConditionName = "[30,50]"
			condition1.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.SUBCONDITION Or EnumConditionChange.ATTRIBUTE)
			condition1.StateChange = EnumConditionState.ACTIVE
			r_conditionList.Add(condition1)
		End Sub

		Public Sub AtSecond50(ByRef r_conditionList As ArrayList)

			condition1.Severity = Convert.ToUInt32(1)
			condition1.Message = "not between [10,50]"
			condition1.AckRequired = False
			condition1.ActiveSubConditionName = [String].Empty
			condition1.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.SUBCONDITION Or EnumConditionChange.ATTRIBUTE)
			condition1.StateChange = EnumConditionState.NO

			condition2.Severity = Convert.ToUInt32(1)
			condition2.Message = "not between [10,50]"
			condition2.AckRequired = False
			condition2.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.ATTRIBUTE)

			condition2.StateChange = EnumConditionState.NO
			r_conditionList.Add(condition1)
			r_conditionList.Add(condition2)

		End Sub

		Public Sub BetweenSecond10and30(ByRef r_conditionList As ArrayList)
			condition1.Severity = Convert.ToUInt32(540)
			condition1.Message = "between [30,50]"
			condition1.AckRequired = True
			condition1.ActiveSubConditionName = "[30,50]"
			condition1.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.SUBCONDITION Or EnumConditionChange.ATTRIBUTE)
			condition1.StateChange = EnumConditionState.ACTIVE

			condition2.Severity = Convert.ToUInt32(510)
			condition2.Message = "between [10,50]"
			condition2.AckRequired = True
			condition2.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.ATTRIBUTE)
			condition2.StateChange = EnumConditionState.ACTIVE
			r_conditionList.Add(condition1)
			r_conditionList.Add(condition2)
		End Sub

		Public Sub BetweenSecond30and50(ByRef r_conditionList As ArrayList)
			condition1.Severity = Convert.ToUInt32(540)
			condition1.Message = "between [30,50]"
			condition1.AckRequired = True
			condition1.ActiveSubConditionName = "[30,50]"
			condition1.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.SUBCONDITION Or EnumConditionChange.ATTRIBUTE)
			condition1.StateChange = EnumConditionState.ACTIVE

			condition2.Severity = Convert.ToUInt32(510)
			condition2.Message = "between [10,50]"
			condition2.AckRequired = True
			condition2.ChangeMask = Convert.ToUInt16(EnumConditionChange.ACTIVE_STATE Or EnumConditionChange.SEVERITY Or EnumConditionChange.MESSAGE Or EnumConditionChange.ATTRIBUTE)
			condition2.StateChange = EnumConditionState.ACTIVE

			r_conditionList.Add(condition1)
			r_conditionList.Add(condition2)
		End Sub

		Public Sub AddSimpleEventSecond(ByRef r_events As ArrayList, ByVal attributeValues As ArrayList)
			Dim secondEvent As New AeEvent(EnumEventType.SIMPLE, "computer/clock/timer", Convert.ToUInt32(CAT_ID_TIME_TICK))
			secondEvent.Severity = Convert.ToUInt32(10)
			secondEvent.ActorId = "sample"
			secondEvent.Message = "Second Changed"
			secondEvent.AttributeValueList = attributeValues
			r_events.Add(secondEvent)
		End Sub

		Public Sub AddSimpleEventMinute(ByRef r_events As ArrayList, ByVal attributeValues As ArrayList)
			Dim minuteEvent As New AeEvent(EnumEventType.SIMPLE, "computer/clock/timer", Convert.ToUInt32(CAT_ID_TIME_TICK))
			minuteEvent.Severity = Convert.ToUInt32(490)
			minuteEvent.ActorId = "sample"
			minuteEvent.Message = "Minute Changed"
			minuteEvent.AttributeValueList = attributeValues
			r_events.Add(minuteEvent)
		End Sub

		Public Sub AddSimpleEventHour(ByRef r_events As ArrayList, ByVal attributeValues As ArrayList)
			Dim hourEvent As New AeEvent(EnumEventType.SIMPLE, "computer/clock/timer", Convert.ToUInt32(CAT_ID_TIME_TICK))
			hourEvent.Severity = Convert.ToUInt32(990)
			hourEvent.ActorId = "sample"
			hourEvent.Message = "Hour Changed"
			hourEvent.AttributeValueList = attributeValues
			r_events.Add(hourEvent)
		End Sub

		'--
#End Region

	End Class
End Namespace

