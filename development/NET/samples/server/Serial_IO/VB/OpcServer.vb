Imports System
Imports System.Collections
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace SerialIO

	Public Class OpcServer

#Region "Constructor"
		'-----------------
		Public Sub New()

		End Sub
		'--
#End Region

#Region "Protected Attributes"
		'--------------------------

		Protected m_shutdownRequest As ShutdownHandler = Nothing
		Protected m_daSimulationElement1 As MyDaAddressSpaceElement = Nothing
		Protected m_daSimulationElement2 As MyDaAddressSpaceElement = Nothing
		Protected m_random As New Random(1)

		Protected m_requestsJanitor As New Object()
		Protected m_requests As New ArrayList()

		'--
#End Region

#Region "Public Properties"
		'-----------------------

		Public Property ServiceName() As String
			Get
				Return Application.Instance.ServiceName
			End Get
			Set(ByVal value As String)
				Application.Instance.ServiceName = value
			End Set
		End Property

		'--
#End Region

#Region "Public Methods"
		'-----------------------

		Public Shared Function Shutdown() As Integer
			Return [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.S_OK)
		End Function

		Public Function Initialize() As System.UInt32
			Try
				Application.Instance.VersionOtb = 447
				Application.Instance.AppType = EnumApplicationType.EXECUTABLE

				Application.Instance.ClsidDa = "{605B206A-6DE0-455b-B4CC-2B3F80324EDC}"
				Application.Instance.ProgIdDa = "Softing.OPC.Toolkit.NET.VB.Sample.SerialIO.DA.1"
				Application.Instance.VerIndProgIdDa = "Softing.OPC.Toolkit.NET.VB.Sample.SerialIO.DA"
				Application.Instance.Description = "Softing OPC Toolkit .NET VB SerialIO Sample Server"
				Application.Instance.MajorVersion = 4
				Application.Instance.MinorVersion = 47
				Application.Instance.PatchVersion = 1
				Application.Instance.BuildNumber = 0
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH"
				Application.Instance.MinUpdateRateDa = 100
				Application.Instance.ClientCheckPeriod = 30000
				Application.Instance.AddressSpaceDelimiter = "."c
				Application.Instance.PropertyDelimiter = "/"c

				Application.Instance.IpPortHTTP = Convert.ToUInt16(8079)
				Application.Instance.UrlDa = "/OPC/DA"
				AddHandler Application.Instance.ShutdownRequest, AddressOf Shutdown
				Application.Instance.EnableTracing([Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL), [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL), _
				[Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER), [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER), "Trace1.txt", System.Convert.ToUInt32(1000000), _
				  System.Convert.ToUInt32(2))
			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "OpcServer::Initialize", exc.ToString())

				Return System.Convert.ToUInt32(EnumResultCode.E_FAIL)
			End Try
			Return System.Convert.ToUInt32(EnumResultCode.S_OK)
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
			Dim result As Integer = Application.Instance.Initialize(aMyCreator)

			Return result
		End Function

		Public Function ProcessCommandLine(ByVal commandLine As String) As Integer
			Return Application.Instance.ProcessCommandLine(commandLine)
		End Function

		Public Function BuildAddressSpace() As System.UInt32
			Try
				Dim creator As MyCreator = CType(Application.Instance.Creator, MyCreator)

				'  DA
				Dim daRoot As DaAddressSpaceRoot = Application.Instance.DaAddressSpaceRoot
				m_daSimulationElement1 = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				m_daSimulationElement1.Name = "simulation1"
				m_daSimulationElement1.AccessRights = EnumAccessRights.READWRITEABLE
				m_daSimulationElement1.Datatype = GetType(Integer)
				m_daSimulationElement1.IoMode = EnumIoMode.POLL
				daRoot.AddChild(m_daSimulationElement1)

				m_daSimulationElement2 = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				m_daSimulationElement2.Name = "simulation2"
				m_daSimulationElement2.AccessRights = EnumAccessRights.READWRITEABLE
				m_daSimulationElement2.Datatype = GetType(Integer)
				m_daSimulationElement2.IoMode = EnumIoMode.POLL
				daRoot.AddChild(m_daSimulationElement2)
			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "OpcServer.BuildAddressSpace", exc.ToString())
				Return Convert.ToUInt32(EnumResultCode.E_FAIL)
			End Try

			Return Convert.ToUInt32(EnumResultCode.S_OK)
		End Function


		Public Sub ChangeSimulationValues()

			If Not m_daSimulationElement1 Is Nothing Then
				m_daSimulationElement1.ValueChanged(New ValueQT(m_random.[Next](5000), [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now))
			End If

			If Not m_daSimulationElement2 Is Nothing Then
				m_daSimulationElement2.ValueChanged(New ValueQT(m_random.[Next](5000), [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now))
			End If

		End Sub

		Public Sub AddRequsts(ByVal requests As ArrayList)
			For Each request As MyRequest In requests
				'  set the requests on pending state. 
				'  they will automatically be set to complete state when complete method is called
				request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.PENDING)
			Next
			SyncLock m_requestsJanitor
				m_requests.AddRange(requests)
			End SyncLock
		End Sub


		Public Sub HandleRequests()
			SyncLock m_requestsJanitor
				For Each request As MyRequest In m_requests
					If request.ProgressRequestState(EnumRequestState.PENDING, EnumRequestState.INPROGRESS) Then
						Dim element As MyDaAddressSpaceElement = CType(request.AddressSpaceElement, MyDaAddressSpaceElement)
						If element Is Nothing Then
							request.Result = EnumResultCode.E_FAIL
						Else
							Select Case request.TransactionType
								Case EnumTransactionType.READ
									If request.PropertyId = 0 Then
										Dim cacheValue As ValueQT = Nothing
										request.AddressSpaceElement.GetCacheValue(cacheValue)
										request.Value = cacheValue
										request.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.S_OK)
									Else
										element.GetPropertyValue(request)
									End If
								Case EnumTransactionType.WRITE
									If request.PropertyId = 0 Then
										Dim newValue As ValueQT = request.Value
										request.Result = [Enum].ToObject(GetType(EnumResultCode), element.ValueChanged(newValue))
									Else
										request.Result = EnumResultCode.E_BADRIGHTS
									End If
								Case Else
							End Select
						End If
					End If
					request.Complete()
				Next
				m_requests.Clear()
			End SyncLock

		End Sub


		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub


		'--
#End Region

	End Class
End Namespace

