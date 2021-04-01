Imports System
Imports System.Collections
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace DllSample

	Public Class OpcServer
#Region "Constants"
		'----------------
		Private Const CATEGORY_ID_SIMULATION As Integer = 1
		Private Const ATTRIBUTE_ID_SIMULATION As Integer = 1
		'--
#End Region

#Region "Constructor"
		'-----------------
		Public Sub New()

		End Sub
		'--
#End Region

#Region "Protected Attributes"
		'--------------------------

		Protected m_shutdownRequest As ShutdownHandler = Nothing
		Protected m_daSimulationElement As MyDaAddressSpaceElement = Nothing
		Protected m_random As New Random(1)
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
			Return [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.S_OK)
		End Function

		Public Function Initialize() As System.UInt32
			Try
				Application.Instance.VersionOtb = 447
				Application.Instance.AppType = EnumApplicationType.EXECUTABLE

				Application.Instance.ClsidDa = "{78C28CD7-2A0C-4BF6-87E9-C4DACA6D9C0F}"
				Application.Instance.ProgIdDa = "Softing.OPC.Toolkit.NET.VB.Sample.Dll.DA.1"
				Application.Instance.VerIndProgIdDa = "Softing.OPC.Toolkit.NET.VB.Sample.Dll.DA"
				Application.Instance.ClsidAe = "{E93EB118-4A2C-4D68-9D7D-BC3525FB67E8}"
				Application.Instance.ProgIdAe = "Softing.OPC.Toolkit.NET.VB.Sample.Dll.AE.1"
				Application.Instance.VerIndProgIdAe = "Softing.OPC.Toolkit.NET.VB.Sample.Dll.AE"
				Application.Instance.Description = "Softing OPC Toolkit .NET VB Dll Sample Server"
				Application.Instance.MajorVersion = 4
				Application.Instance.MinorVersion = 47
				Application.Instance.PatchVersion = 1
				Application.Instance.BuildNumber = 0
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH"
				Application.Instance.MinUpdateRateDa = 100
				Application.Instance.ClientCheckPeriod = 30000
				Application.Instance.AddressSpaceDelimiter = "."c
				Application.Instance.PropertyDelimiter = "/"c
				Application.Instance.SupportDisableConditions = True

				Application.Instance.IpPortHTTP = Convert.ToUInt16(8079)
				Application.Instance.UrlDa = "/OPC/DA"
				AddHandler Application.Instance.ShutdownRequest, AddressOf Shutdown
				Application.Instance.EnableTracing([Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL), [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL), _
				[Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER), [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER), "Trace.txt", System.Convert.ToUInt32(1000000), _
				  System.Convert.ToUInt32(0))
			Catch exc As Exception
				Trace( _
				 EnumTraceLevel.ERR, _
				 [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), _
				 "OpcServer::Initialize", _
				 exc.ToString)
				Return Convert.ToUInt32(EnumResultCode.E_FAIL)
			End Try
			Return Convert.ToUInt32(EnumResultCode.S_OK)
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
			Return Application.Instance.Initialize(aMyCreator)
		End Function

		Public Function ProcessCommandLine(ByVal commandLine As String) As Integer
			Return Application.Instance.ProcessCommandLine(commandLine)
		End Function

		Public Function BuildAddressSpace() As System.UInt32
			Try
				Dim creator As MyCreator = DirectCast(Application.Instance.Creator, MyCreator)

				'  DA        
				Dim daRoot As DaAddressSpaceRoot = Application.Instance.DaAddressSpaceRoot
				m_daSimulationElement = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				m_daSimulationElement.Name = "simulation"
				m_daSimulationElement.AccessRights = EnumAccessRights.READWRITEABLE
				m_daSimulationElement.Datatype = GetType(System.UInt32)
				m_daSimulationElement.IoMode = EnumIoMode.POLL
				daRoot.AddChild(m_daSimulationElement)

				m_daSimulationElement.ValueChanged(New ValueQT(m_random.[Next](5000), [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now))
				'AE
				Dim aeRoot As AeAddressSpaceRoot = Application.Instance.AeAddressSpaceRoot
				Dim aeElement As AeAddressSpaceElement = creator.CreateAeAddressSpaceElement()
				aeElement.Name = "SimulationElement"
				aeElement.HasChildren = False
				aeRoot.AddChild(aeElement)
			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "OpcServer:;BuildAddressSpace", exc.ToString())
				Return System.Convert.ToUInt32(EnumResultCode.E_FAIL)
			End Try

			Return System.Convert.ToUInt32(EnumResultCode.S_OK)
		End Function
		Public Function BuildEventCategories() As System.UInt32
			Try
				Dim simulationCategory As New AeCategory(EnumEventType.SIMPLE, Convert.ToUInt32(CATEGORY_ID_SIMULATION), "SimulationCategory")

				Application.Instance.AddAeCategory(simulationCategory)
				simulationCategory.AddAttribute(Convert.ToUInt32(ATTRIBUTE_ID_SIMULATION), "SimulationAttribute", GetType(System.UInt16))
			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "OpcServer::BuildEventCategories", exc.ToString())
				Return Convert.ToUInt32(EnumResultCode.E_FAIL)
			End Try

			Return Convert.ToUInt32(EnumResultCode.S_OK)

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
				m_daSimulationElement.ValueChanged(New ValueQT(m_random.[Next](5000), [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now))

			End If
		End Sub
		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub


		'--
#End Region

	End Class

End Namespace

