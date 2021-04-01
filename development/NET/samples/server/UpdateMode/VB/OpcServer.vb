Imports System
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace UpdateMode
	Public Enum EnumDaVariableType
		SEC = 0
		MIN
	End Enum

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
		Private m_reportTagSec As MyDaAddressSpaceElement = Nothing
		Private m_reportTagMin As MyDaAddressSpaceElement = Nothing

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

				Application.Instance.ClsidDa = "{F181ECE9-641E-41E0-99B9-CA5AE722CFDE}"
				Application.Instance.ProgIdDa = "Softing.OPC.Toolkit.NET.VB.Sample.UpdateMode.DA.1"
				Application.Instance.VerIndProgIdDa = "Softing.OPC.Toolkit.NET.VB.Sample.UpdateMode.DA"
				Application.Instance.Description = "Softing OPC Toolkit .NET VB UpdateMode Sample Server"

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
				[Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER), [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER), "Trace.txt", Convert.ToUInt32(1000000), _
				  Convert.ToUInt32(2))

			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "OpcServer::Initialize", exc.ToString())

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
				Dim root As DaAddressSpaceRoot = Application.Instance.DaAddressSpaceRoot
				Dim creator As MyCreator = CType(Application.Instance.Creator, MyCreator)

				Dim clockNode As MyDaAddressSpaceElement = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				clockNode.Name = "clock"
				clockNode.Type = MyDaAddressSpaceElement.TYPE_NODECLOCK
				clockNode.IoMode = EnumIoMode.NONE
				clockNode.HasChildren = True
				root.AddChild(clockNode)

				Dim reportNode As MyDaAddressSpaceElement = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				reportNode.Name = "report"
				reportNode.Type = MyDaAddressSpaceElement.TYPE_NODEREPORT
				reportNode.IoMode = EnumIoMode.NONE
				reportNode.HasChildren = True
				clockNode.AddChild(reportNode)

				m_reportTagSec = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				m_reportTagSec.Name = "sec"
				m_reportTagSec.Type = MyDaAddressSpaceElement.TYPE_REPORT_SEC
				m_reportTagSec.AccessRights = EnumAccessRights.READABLE
				m_reportTagSec.Datatype = GetType(Int32)
				m_reportTagSec.IoMode = EnumIoMode.REPORT_CYCLIC
				reportNode.AddChild(m_reportTagSec)

				m_reportTagMin = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				m_reportTagMin.Name = "min"
				m_reportTagMin.Type = MyDaAddressSpaceElement.TYPE_REPORT_MIN
				m_reportTagMin.AccessRights = EnumAccessRights.READABLE
				m_reportTagMin.Datatype = GetType(Int32)
				m_reportTagMin.IoMode = EnumIoMode.REPORT_CYCLIC
				reportNode.AddChild(m_reportTagMin)

				Dim pollNode As MyDaAddressSpaceElement = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				pollNode.Name = "poll"
				pollNode.Type = MyDaAddressSpaceElement.TYPE_NODEPOLL
				pollNode.IoMode = EnumIoMode.NONE
				pollNode.HasChildren = True
				clockNode.AddChild(pollNode)

				Dim tag As MyDaAddressSpaceElement = Nothing

				tag = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				tag.Name = "sec"
				tag.Type = MyDaAddressSpaceElement.TYPE_POLL_SEC
				tag.AccessRights = EnumAccessRights.READABLE
				tag.Datatype = GetType(Int32)
				tag.IoMode = EnumIoMode.POLL
				pollNode.AddChild(tag)

				tag = CType(creator.CreateMyDaAddressSpaceElement(), MyDaAddressSpaceElement)
				tag.Name = "min"
				tag.Type = MyDaAddressSpaceElement.TYPE_POLL_MIN
				tag.AccessRights = EnumAccessRights.READABLE
				tag.Datatype = GetType(Int32)
				tag.IoMode = EnumIoMode.POLL
				pollNode.AddChild(tag)

			Catch exc As Exception
				Trace(EnumTraceLevel.ERR, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), "OpcServer.BuildAddressSpace", exc.ToString())
				Return Convert.ToUInt32(EnumResultCode.E_FAIL)
			End Try

			Return Convert.ToUInt32(EnumResultCode.S_OK)
		End Function
		Public Sub ChangeValue(ByVal varValue As Object, ByVal variableType As EnumDaVariableType)
			Dim val As New ValueQT(varValue, [Enum].ToObject(GetType(EnumQuality), EnumQuality.GOOD), DateTime.Now)

			Select Case variableType
				Case EnumDaVariableType.SEC
					If Not m_reportTagSec Is Nothing Then
						m_reportTagSec.ValueChanged(val)
					End If

				Case EnumDaVariableType.MIN
					If Not m_reportTagMin Is Nothing Then
						m_reportTagMin.ValueChanged(val)
					End If

				Case Else
			End Select

		End Sub
		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub


		'--
#End Region

	End Class
End Namespace

