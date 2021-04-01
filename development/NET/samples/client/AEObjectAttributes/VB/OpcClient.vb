Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace AEObjectAttributes
	Public Class OpcClient

#Region "Fields"
		Dim m_subscriptionFilter As EnumObjectAttribute
		Dim m_subscriptionMaxsize As EnumObjectAttribute
		Dim m_subscriptionBuffertime As EnumObjectAttribute
		Dim m_subscriptionReturnedAttributes As EnumObjectAttribute
#End Region

#Region "Constructor"
		'-----------------

		Public Sub New()
			m_subscriptionFilter = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.AESUBSCRIPTION_FILTER)
			m_subscriptionMaxsize = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.AESUBSCRIPTION_MAXSIZE)
			m_subscriptionBuffertime = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.AESUBSCRIPTION_BUFFERTIME)
			m_subscriptionReturnedAttributes = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.AESUBSCRIPTION_RETURNED_ATTRIBUTES)
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
			GetApplication().EnableTracing(traceGroupClient, traceGroupAll, traceGroupClient, traceGroupClient, "Trace.txt", Convert.ToUInt32(1000000), _
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


		Public Function InitializeAeObjects() As Integer
			Dim connectResult As Integer = EnumResultCode.E_FAIL
			m_executionOptions = New ExecutionOptions
			m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS
			m_executionOptions.ExecutionContext = Convert.ToUInt32(0)

			Try

				m_aeSession = New MyAeSession("opcae:///Softing.OPCToolboxDemo_ServerAE.1/{2E565243-B238-11D3-842D-0008C779D775}")
				m_aeSubscription = New MyAeSubscription(m_aeSession)

				connectResult = m_aeSession.Connect(True, True, New ExecutionOptions)
			Catch exc As Exception
				Dim traceGroupUser As EnumTraceGroup
				traceGroupUser = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER)
				GetApplication().Trace(EnumTraceLevel.ERR, traceGroupUser, "OpcClient::InitializeAeObjects", exc.ToString())
			End Try

			Return connectResult
		End Function

		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub

		Public Sub PrintAeSession()

			System.Console.WriteLine("Session Attributes:")
			System.Console.WriteLine("  URL: " + m_aeSession.Url)
			System.Console.WriteLine("  ClientName: " + m_aeSession.ClientName)
			System.Console.WriteLine("  LocaleID: " + m_aeSession.LocaleId)
		End Sub

		Public Sub PrintAeSubscription()

			System.Console.WriteLine("Subscription Attributes:")
			System.Console.WriteLine("  RequestedBufferTime: " + m_aeSubscription.RequestedBufferTime.ToString())
			System.Console.WriteLine("  RevisedBufferTime: " + m_aeSubscription.RevisedBufferTime.ToString())

			System.Console.WriteLine("  RequestedMaxSize: " + m_aeSubscription.RequestedMaxSize.ToString() + " events")
			System.Console.WriteLine("  RevisedMaxSize: " + m_aeSubscription.RevisedMaxSize.ToString() + " events")

			System.Console.WriteLine("  Filter: ")
			System.Console.WriteLine("    LowSeverity: " + m_aeSubscription.FilterSeverityLow.ToString())
			System.Console.WriteLine("    HighSeverity: " + m_aeSubscription.FilterSeverityHigh.ToString())
			System.Console.WriteLine("    EventTypes: " + m_aeSubscription.FilterEventTypes.ToString())

			Dim categories As UInt32() = m_aeSubscription.FilterCategories
			System.Console.WriteLine("  Categories: count: " + categories.Length.ToString())
			Dim i As Integer = 0
			While i < categories.Length
				System.Console.WriteLine("   CategoryID: " + categories(i).ToString())
				System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
			End While

			Dim areas As String() = m_aeSubscription.FilterAreas
			System.Console.WriteLine("  Areas: count: " + areas.Length.ToString())
			Dim m As Integer = 0
			While m < areas.Length
				System.Console.WriteLine("   Area: " + areas(m))
				System.Math.Max(System.Threading.Interlocked.Increment(m), m - 1)
			End While

			Dim sources As String() = m_aeSubscription.FilterSources
			System.Console.WriteLine("  Sources: count: " + sources.Length.ToString())
			Dim j As Integer = 0
			While j < sources.Length
				System.Console.WriteLine("   Area: " + sources(j))
				System.Math.Max(System.Threading.Interlocked.Increment(j), j - 1)
			End While

			Dim attributes As AeReturnedAttributes() = m_aeSubscription.ReturnedAttributes

			System.Console.WriteLine("        Returned Attributes: count: " + attributes.Length.ToString())
			Dim k As Integer = 0
			While k < attributes.Length
				If Not attributes(k).AttributeIds Is Nothing Then
					System.Console.WriteLine("      Attributes IDs: count: " + attributes(k).AttributeIds.Length.ToString())
					Dim l As Integer = 0
					While l < attributes(k).AttributeIds.Length
						System.Console.WriteLine("      Attribute ID: " + attributes(k).AttributeIds(l).ToString())
						System.Math.Max(System.Threading.Interlocked.Increment(l), l - 1)
					End While
				End If
				System.Math.Max(System.Threading.Interlocked.Increment(k), k - 1)
			End While
		End Sub

		Public Function ChangeSession() As Integer
			Dim updateResults As Integer() = New Integer(1) {}
			Dim operationResult As Integer
			Dim sessionClientname As EnumObjectAttribute
			Dim sessionLcid As EnumObjectAttribute
			sessionClientname = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.AESESSION_CLIENTNAME)
			sessionLcid = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.AESESSION_LCID)
			'GetApplication().UpdateAttributesBehavior = EnumUpdateAttributesBehavior.EXPLICIT_UPDATE;                                                                        

			m_aeSession.ClientName = "AE ObjectAttributes Sample"
			m_aeSession.LocaleId = "de-DE"

			' write changed data to the server 
			operationResult = m_aeSession.SetAttributesToServer(New EnumObjectAttribute() {sessionClientname, sessionLcid}, updateResults, New ExecutionOptions)

			Return operationResult
		End Function

		Public Function CheckSession() As Integer
			Dim updateResults As Integer() = New Integer(1) {}
			Dim operationResult As Integer
			Dim sessionAll As EnumObjectAttribute
			sessionAll = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.AESESSION_ALL)


			' get it from the server to check 
			operationResult = m_aeSession.GetAttributesFromServer(New EnumObjectAttribute() {sessionAll}, updateResults, New ExecutionOptions)

			Return operationResult
		End Function

		Public Function ChangeSubscription() As Integer
			Dim updateResults As Integer() = New Integer(1) {}
			Dim operationResult As Integer

			m_aeSubscription.RequestedBufferTime = Convert.ToUInt32(2000)
			m_aeSubscription.RequestedMaxSize = Convert.ToUInt32(5)
			m_aeSubscription.FilterSeverityLow = Convert.ToUInt32(100)
			m_aeSubscription.FilterSeverityHigh = Convert.ToUInt32(700)
			m_aeSubscription.FilterEventTypes = Convert.ToUInt32(EnumEventType.SIMPLE Or EnumEventType.TRACKING)
			m_aeSubscription.FilterCategories = New UInt32() {Convert.ToUInt32(1), Convert.ToUInt32(2), Convert.ToUInt32(3)}
			m_aeSubscription.FilterAreas = New String() {"computer.mouse", "computer.clock"}
			m_aeSubscription.FilterSources = New String() {"computer.clock.time slot 2", "computer.clock.time slot 1"}

			' write changed data to the server     
			operationResult = m_aeSubscription.SetAttributesToServer(New EnumObjectAttribute() {m_subscriptionFilter, m_subscriptionMaxsize, m_subscriptionBuffertime}, updateResults, New ExecutionOptions)

			Return operationResult
		End Function

		Public Function CheckSubscription() As Integer
			Dim updateResults As Integer() = New Integer(1) {}
			Dim operationResult As Integer

			' get it from the server to check 
			operationResult = m_aeSubscription.GetAttributesFromServer(New EnumObjectAttribute() {m_subscriptionFilter, m_subscriptionMaxsize, m_subscriptionBuffertime}, updateResults, New ExecutionOptions)

			Return operationResult
		End Function

		Public Function ChangeSubscriptionAttributes() As Integer
			Dim updateResults As Integer() = New Integer(1) {}
			Dim operationResult As Integer

			Dim attrs1 As New AeReturnedAttributes
			attrs1.CategoryId = Convert.ToUInt32(2)
			attrs1.AttributeIds = New UInt32() {Convert.ToUInt32(3), Convert.ToUInt32(4)}
			Dim attrs2 As New AeReturnedAttributes
			attrs2.CategoryId = Convert.ToUInt32(3)
			attrs2.AttributeIds = New UInt32() {Convert.ToUInt32(6)}
			m_aeSubscription.ReturnedAttributes = New AeReturnedAttributes() {attrs1, attrs2}

			' write changed data to the server       
			operationResult = m_aeSubscription.SetAttributesToServer(New EnumObjectAttribute() {m_subscriptionReturnedAttributes}, updateResults, New ExecutionOptions)

			Return operationResult
		End Function


		Public Function CheckSubscriptionAttributes() As Integer
			Dim updateResults As Integer() = New Integer(1) {}
			Dim operationResult As Integer

			operationResult = m_aeSubscription.GetAttributesFromServer(New EnumObjectAttribute() {m_subscriptionReturnedAttributes}, updateResults, New ExecutionOptions)

			Return operationResult
		End Function


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

