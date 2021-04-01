Imports System
Imports System.Collections
Imports System.Threading
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace AddressSpace
	Public Class OpcServer
#Region "Constructor"
		'-----------------
		Public Sub New()

		End Sub
		'--
#End Region

#Region "Public Attributes"
		'-----------------------

		Public Shared AddressSpaceChanged As AutoResetEvent = Nothing
		Public ReconfigHandles As MyDaAddressSpaceElement() = Nothing
		Public ObjectBased As MyDaAddressSpaceElement = Nothing

		'-
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

		Public Property ServiceDescription() As String
			Get
				Return Application.Instance.ServiceDescription
			End Get
			Set(ByVal value As String)
				Application.Instance.ServiceDescription = value
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
				Application.Instance.ClsidDa = "{8532429A-DA67-4c26-B045-71084F3A23D2}"
				Application.Instance.ProgIdDa = "Softing.OPC.Toolkit.NET.VB.AddressSpace.DA.1"
				Application.Instance.VerIndProgIdDa = "Softing.OPC.Toolkit.NET.VB.AddressSpace.DA"
				Application.Instance.Description = "Softing OPC Toolkit .NET VB AddressSpace Sample Server"
				Application.Instance.IpPortHTTP = Convert.ToUInt16(8079)
				Application.Instance.UrlDa = "/OPC/DA"
				Application.Instance.MajorVersion = 4
				Application.Instance.MinorVersion = 47
				Application.Instance.PatchVersion = 1
				Application.Instance.BuildNumber = 0
				Application.Instance.VendorInfo = "Softing Industrial Automation GmbH"
				Application.Instance.MinUpdateRateDa = 100
				Application.Instance.ServiceName = ""
				Application.Instance.ClientCheckPeriod = 30000
				Application.Instance.AddressSpaceDelimiter = "."c
				Application.Instance.PropertyDelimiter = "#"c
				Dim traceGroupAll As EnumTraceGroup = CType([Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL), EnumTraceGroup)
				Dim traceGroupServer As EnumTraceGroup = CType([Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER), EnumTraceGroup)

				Application.Instance.EnableTracing(traceGroupAll, traceGroupAll, traceGroupServer, traceGroupServer, "c:\\Trace.txt", Convert.ToUInt32(1000000),
	  Convert.ToUInt32(0))

				AddHandler Application.Instance.ShutdownRequest, AddressOf Shutdown
			Catch exc As Exception
				Dim traceGroup As EnumTraceGroup = CType([Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER1), EnumTraceGroup)
				Trace(EnumTraceLevel.ERR, traceGroup, "OpcServer::Initialize", exc.ToString())

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
			'  firstly activate the COM-DA Server feature
			'  result = Application.Instance.Activate(EnumFeature.DA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX")
			If Not ResultCode.SUCCEEDED(result) Then
				Return result
			End If

			'  activate the XML-DA Server Feature
			'  result = Application.Instance.Activate(EnumFeature.XMLDA_SERVER, "XXXX-XXXX-XXXX-XXXX-XXXX")
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

		Public Sub BuildAddressSpace()
			Dim rand As New Random()
			Dim tag As MyDaAddressSpaceElement

			ObjectBased = New MyDaAddressSpaceElement()
			ObjectBased.Name = "ObjectBased"
			ObjectBased.IoMode = EnumIoMode.NONE
			ObjectBased.HasChildren = True
			ObjectBased.ObjectType = 1

			Application.Instance.DaAddressSpaceRoot.AddChild(ObjectBased)

			tag = New MyDaAddressSpaceElement()
			tag.Name = "T1"
			tag.AccessRights = EnumAccessRights.READWRITEABLE
			tag.Datatype = GetType(Byte)
			tag.IoMode = EnumIoMode.POLL
			tag.ObjectType = 2
			ObjectBased.AddChild(tag)

			tag = New MyDaAddressSpaceElement()
			tag.Name = "T2"
			tag.AccessRights = EnumAccessRights.READWRITEABLE
			tag.Datatype = GetType(Byte)
			tag.IoMode = EnumIoMode.POLL
			tag.ObjectType = 2
			ObjectBased.AddChild(tag)

			tag = New MyDaAddressSpaceElement()
			tag.Name = "T3"
			tag.AccessRights = EnumAccessRights.READWRITEABLE
			tag.Datatype = GetType(Byte)
			tag.IoMode = EnumIoMode.POLL
			tag.ObjectType = 2
			ObjectBased.AddChild(tag)

			ReconfigHandles(0) = New MyDaAddressSpaceElement()
			ReconfigHandles(0).Name = [String].Format("R{0}", rand.[Next]())
			ReconfigHandles(0).AccessRights = EnumAccessRights.READWRITEABLE
			ReconfigHandles(0).Datatype = GetType(Int32)
			ReconfigHandles(0).IoMode = EnumIoMode.REPORT
			ReconfigHandles(0).HasChildren = True
			ReconfigHandles(0).ObjectType = 3
			ObjectBased.AddChild(ReconfigHandles(0))

			ReconfigHandles(1) = New MyDaAddressSpaceElement()
			ReconfigHandles(1).Name = [String].Format("R{0}", rand.[Next]())
			ReconfigHandles(1).AccessRights = EnumAccessRights.READWRITEABLE
			ReconfigHandles(1).Datatype = GetType(Int32)
			ReconfigHandles(1).IoMode = EnumIoMode.REPORT
			ReconfigHandles(1).ObjectType = 4
			ObjectBased.AddChild(ReconfigHandles(1))

			ReconfigHandles(2) = New MyDaAddressSpaceElement()
			ReconfigHandles(2).Name = [String].Format("R{0}", rand.[Next]())
			ReconfigHandles(2).AccessRights = EnumAccessRights.READWRITEABLE
			ReconfigHandles(2).Datatype = GetType(Int32)
			ReconfigHandles(2).IoMode = EnumIoMode.REPORT
			ReconfigHandles(2).ObjectType = 5
			ReconfigHandles(0).AddChild(ReconfigHandles(2))

		End Sub

		Public Sub RebuildRandomAddressSpace(ByVal objBase As AddressSpaceElement)
			If objBase Is Nothing Then
				Return
			End If
			If Not objBase.Name.Equals("ObjectBased") Then
				Return
			End If

			Dim rand As New Random()

			ReconfigHandles(0) = New MyDaAddressSpaceElement()
			ReconfigHandles(0).Name = [String].Format("R{0}", rand.[Next]())
			ReconfigHandles(0).AccessRights = EnumAccessRights.READWRITEABLE
			ReconfigHandles(0).Datatype = GetType(Int32)
			ReconfigHandles(0).IoMode = EnumIoMode.REPORT
			ReconfigHandles(0).HasChildren = True
			ReconfigHandles(0).ObjectType = 3
			objBase.AddChild(ReconfigHandles(0))

			ReconfigHandles(1) = New MyDaAddressSpaceElement()
			ReconfigHandles(1).Name = [String].Format("R{0}", rand.[Next]())
			ReconfigHandles(1).AccessRights = EnumAccessRights.READWRITEABLE
			ReconfigHandles(1).Datatype = GetType(Int32)
			ReconfigHandles(1).IoMode = EnumIoMode.REPORT
			ReconfigHandles(1).ObjectType = 4
			objBase.AddChild(ReconfigHandles(1))

			ReconfigHandles(2) = New MyDaAddressSpaceElement()
			ReconfigHandles(2).Name = [String].Format("R{0}", rand.[Next]())
			ReconfigHandles(2).AccessRights = EnumAccessRights.READWRITEABLE
			ReconfigHandles(2).Datatype = GetType(Int32)
			ReconfigHandles(2).IoMode = EnumIoMode.REPORT
			ReconfigHandles(2).ObjectType = 5
			ReconfigHandles(0).AddChild(ReconfigHandles(2))
		End Sub

		Public Sub ShowObjectTree()

			System.Console.WriteLine("CURRENT ADDRESS SPACE")
			Dim root As IAddressSpaceElement = Application.Instance.DaAddressSpaceRoot
			Dim level As Integer = 0

			PrintChildren(root, level)

			System.Console.WriteLine([String].Format("Press Ctrl-C to exit"))
		End Sub

		Public Sub PrintChildren(ByVal element As IAddressSpaceElement, ByRef bLevel As Integer)

			Dim children As ArrayList = element.GetChildren()

			For i As Integer = 0 To children.Count - 1

				Dim child As MyDaAddressSpaceElement = DirectCast(children(i), MyDaAddressSpaceElement)

				If Not children(i) Is Nothing Then

					Dim line As String = String.Empty
					Dim j As Integer = 0
					While j < bLevel
						line += String.Format("  ")
						j = j + 1
					End While

					System.Console.WriteLine(String.Format("{0}[{1}]", line, child.Name))

					If child.HasChildren = True Then
						bLevel = bLevel + 1
						PrintChildren(DirectCast(children(i), IAddressSpaceElement), bLevel)
						bLevel = bLevel - 1
					End If
				End If
			Next i

		End Sub

		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub


		'--
#End Region

	End Class
End Namespace

