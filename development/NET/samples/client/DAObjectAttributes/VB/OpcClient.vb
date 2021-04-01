Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DAObjectAttributes
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
		Private m_daSubscription As MyDaSubscription = Nothing
		Private m_daItem1 As MyDaItem = Nothing
		Private m_daItem2 As MyDaItem = Nothing
		Private m_daItem3 As MyDaItem = Nothing
		Private m_executionOptions As ExecutionOptions = Nothing
		Private m_itemList As MyDaItem() = Nothing
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

			Dim traceGroupAll As EnumTraceGroup
			Dim traceGroupClient As EnumTraceGroup
			traceGroupAll = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL)
			traceGroupClient = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.CLIENT)

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
			If m_daSubscription.CurrentState <> EnumObjectState.DISCONNECTED Then
				m_daSubscription.Disconnect(New ExecutionOptions)
			End If
			If m_daSession.CurrentState <> EnumObjectState.DISCONNECTED Then
				m_daSession.Disconnect(New ExecutionOptions)
			End If

			m_daSubscription.RemoveDaItem(m_daItem1)
			m_daSubscription.RemoveDaItem(m_daItem2)
			m_daSubscription.RemoveDaItem(m_daItem3)

			m_daSession.RemoveDaSubscription(m_daSubscription)
			GetApplication().RemoveDaSession(m_daSession)
			GetApplication().Terminate()
			m_daSession = Nothing
			m_daSubscription = Nothing
			m_daItem1 = Nothing
			m_daItem2 = Nothing
			m_daItem3 = Nothing
			m_executionOptions = Nothing
			m_itemList = Nothing
		End Sub


		Public Function InitializeDaObjects() As Integer
			Dim connectResult As Integer = EnumResultCode.E_FAIL
			m_itemList = New MyDaItem(2) {}
			m_executionOptions = New ExecutionOptions
			Dim traceGroupUser As EnumTraceGroup
			traceGroupUser = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER)

			Try

				'  TODO add your server URL here
				'  this is the server url for Softing OPCToolbox Demo Server
				'  first choice is COM-DA 
				Dim url As String = "opcda://localhost/Softing.OPCToolboxDemo_ServerDA/{2e565242-b238-11d3-842d-0008c779d775}"

				'uncomment if you need an XML-DA access
				'  Dim url As String = "http://localhost:8079/OPC/DA"

				m_daSession = New MyDaSession(url)

				m_daSubscription = New MyDaSubscription(Convert.ToUInt32(500), m_daSession)

				m_daItem1 = New MyDaItem("maths.sin", m_daSubscription)
				m_itemList(0) = m_daItem1

				m_daItem2 = New MyDaItem("time.local.second", m_daSubscription)
				m_itemList(1) = m_daItem2

				m_daItem3 = New MyDaItem("increment.UI1", m_daSubscription)
				m_itemList(2) = m_daItem3

				connectResult = m_daSession.Connect(True, False, m_executionOptions)

			Catch exc As Exception
				GetApplication().Trace(EnumTraceLevel.ERR, traceGroupUser, "OpcClient::InitializaDaObjects", exc.ToString())
			End Try

			Return connectResult
		End Function


		Public Sub PrintDaSession(ByVal session As DaSession)

			System.Console.WriteLine("Session's Attributes:")
			System.Console.WriteLine("  URL: " + session.Url)
			System.Console.WriteLine("  ClientName: " + session.ClientName)
			System.Console.WriteLine("  LocaleID: " + session.LocaleId)
			System.Console.WriteLine("  SupportedOPCSpecification: " + session.SupportedOpcSpecification.ToString())
			System.Console.WriteLine("  Forced OPC Specification: " + session.ForcedOpcSpecification.ToString())
			System.Console.WriteLine("  Class context: " + session.ClassContext.ToString())
			System.Console.WriteLine()
		End Sub

		Public Sub PrintDaSubscription(ByVal subscription As DaSubscription)

			System.Console.WriteLine("Subscription's Attributes:")
			System.Console.WriteLine("  Name: " + subscription.Name)
			System.Console.WriteLine("  RequestedUpdateRate: " + subscription.RequestedUpdateRate.ToString())
			System.Console.WriteLine("  RevisedUpdateRate: " + subscription.RevisedUpdateRate.ToString())
			System.Console.WriteLine("  Deadband: " + subscription.Deadband.ToString())
			System.Console.WriteLine("  TimeBias: " + subscription.TimeBias.ToString())
			System.Console.WriteLine("  RequestedKeepAliveTime: " + subscription.RequestedKeepAliveTime.ToString())
			System.Console.WriteLine("  RevisedKeepAliveTime: " + subscription.RevisedKeepAliveTime.ToString())
			System.Console.WriteLine("  LocaleID: " + subscription.LocaleId)
			System.Console.WriteLine()

		End Sub

		Public Sub PrintDaItem(ByVal item As DaItem)

			System.Console.WriteLine("Item's Attributes:")
			System.Console.WriteLine("  ID: " + item.Id)
			System.Console.WriteLine("  Path: " + item.Path)
			System.Console.WriteLine("  NativeDatatype: " + item.NativeDatatype.ToString())
			System.Console.WriteLine("  AccessRights: " + item.AccessRights.ToString())
			System.Console.WriteLine("  RequestedDatatype: " + item.RequestedDatatype.ToString())
			System.Console.WriteLine("  Deadband: " + item.Deadband.ToString())
			System.Console.WriteLine("  EUType: " + item.EUType.ToString())
			If Not item.EUInfo Is Nothing Then
				Dim euValue As ValueData
				euValue = New ValueData(item.EUInfo)
				System.Console.WriteLine("  EUInfo: " + euValue.ToString())
			End If
			System.Console.WriteLine()
		End Sub

		Public Sub PrintInitialState()
			System.Console.WriteLine("--- Initial object attributes ---")
			PrintDaSession(m_daSession)
			PrintDaSubscription(m_daSubscription)
			PrintDaItem(m_daItem1)
			PrintDaItem(m_daItem2)
			PrintDaItem(m_daItem3)
		End Sub

		Public Sub ChangeSession()
			Dim operationResult As Integer
			Dim results As Integer() = Nothing
			Dim sessionLCID As EnumObjectAttribute
			Dim sessionClientname As EnumObjectAttribute
			sessionLCID = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.DASESSION_LCID)
			sessionClientname = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.DASESSION_CLIENTNAME)


			System.Console.WriteLine("--- Change the session's LCID and client name ---")
			System.Console.WriteLine()

			m_daSession.LocaleId = "de-DE"
			m_daSession.ClientName = "ObjectAttributes Sample"

			' write changed data to the server 
			operationResult = m_daSession.SetAttributesToServer(New EnumObjectAttribute() {sessionLCID, sessionClientname}, results, m_executionOptions)

			System.Console.WriteLine("Update session's attributes to server - result " + operationResult.ToString())

			' get it from the server to check 
			operationResult = m_daSession.GetAttributesFromServer(New EnumObjectAttribute() {sessionLCID, sessionClientname}, results, m_executionOptions)

			System.Console.WriteLine("Update session's attributes from server - result " + operationResult.ToString())

			PrintDaSession(m_daSession)
		End Sub

		Public Sub ChangeSubscription()
			Dim operationResult As Integer
			Dim results As Integer() = Nothing
			Dim subscriptionName As EnumObjectAttribute
			Dim subscriptionDeadband As EnumObjectAttribute
			Dim subscriptionKeepalive As EnumObjectAttribute
			Dim subscriptionUpdaterate As EnumObjectAttribute
			subscriptionName = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.DASUBSCRIPTION_NAME)
			subscriptionDeadband = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.DASUBSCRIPTION_DEADBAND)
			subscriptionKeepalive = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.DASUBSCRIPTION_KEEPALIVETIME)
			subscriptionUpdaterate = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.DASUBSCRIPTION_UPDATERATE)

			System.Console.WriteLine("--- Change the subscription's name, update rate, deadband and keep alive time ---")
			System.Console.WriteLine()

			m_daSubscription.Name = "ObjectAttributes Subscription"
			m_daSubscription.RequestedUpdateRate = Convert.ToUInt32(1000)
			m_daSubscription.RequestedKeepAliveTime = Convert.ToUInt32(4000)
			m_daSubscription.Deadband = 10.0F

			' write changed data to the server 
			operationResult = m_daSubscription.SetAttributesToServer(New EnumObjectAttribute() {subscriptionName, subscriptionUpdaterate, subscriptionDeadband, subscriptionKeepalive}, results, m_executionOptions)

			System.Console.WriteLine("Update subscription's attributes to server - result " + operationResult.ToString())

			' get it from the server to check 
			operationResult = m_daSubscription.GetAttributesFromServer(New EnumObjectAttribute() {subscriptionName, subscriptionUpdaterate, subscriptionDeadband, subscriptionKeepalive}, results, m_executionOptions)

			System.Console.WriteLine("Update subscription's attributes from server - result " + operationResult.ToString())

			System.Console.WriteLine()
			PrintDaSubscription(m_daSubscription)
		End Sub

		Public Sub ChangeItemDatatype()

			Dim requestedDatatype As EnumObjectAttribute
			requestedDatatype = [Enum].ToObject(GetType(EnumObjectAttribute), EnumObjectAttribute.DAITEM_REQUESTED_DATATYPE)

			System.Console.WriteLine("--- Change requested datatype of the items ---")

			Dim a As String = String.Empty
			Dim operationResult As Integer
			Dim results As Integer() = Nothing
			m_daItem1.RequestedDatatype = a.[GetType]()
			m_daItem2.RequestedDatatype = a.[GetType]()
			m_daItem3.RequestedDatatype = a.[GetType]()

			' write requested datatypes to the server 
			operationResult = m_daSubscription.SetDaItemAttributesToServer(m_itemList, New EnumObjectAttribute(0) {requestedDatatype}, results, m_executionOptions)

			System.Console.WriteLine("Update item's attributes to server - result " + operationResult.ToString())

			' get it from the server to check 
			m_daSubscription.GetDaItemAttributesFromServer(m_itemList, New EnumObjectAttribute(0) {requestedDatatype}, results, m_executionOptions)

			System.Console.WriteLine("Update item's attributes from server - result " + operationResult.ToString())

			PrintDaItem(m_daItem1)
			PrintDaItem(m_daItem2)
			PrintDaItem(m_daItem3)
		End Sub

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

