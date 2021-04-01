Imports System
Imports System.Threading
Imports System.Collections
Imports System.Runtime.InteropServices
Imports Softing.OPCToolbox
Imports Softing.OPCToolbox.Server

Namespace BISDemo

#Region "//  Console control handling"

	' A simple class that exposes two static Win32 functions.
	' One is a delegate type and the other is an enumerated type.
	Public Class MyWin32
		' Declare the SetConsoleCtrlHandler function 
		' as external and receiving a delegate.   
		<DllImport("Kernel32")> _
		Public Shared Function SetConsoleCtrlHandler(ByVal Handler As HandlerRoutine, ByVal Add As Boolean) As Boolean
		End Function

		' A delegate type to be used as the handler routine 
		' for SetConsoleCtrlHandler.
		Public Delegate Function HandlerRoutine(ByVal CtrlType As CtrlTypes) As Boolean

		' An enumerated type for the control messages 
		' sent to the handler routine.
		Public Enum CtrlTypes
			CTRL_C_EVENT = 0
			CTRL_BREAK_EVENT
			CTRL_CLOSE_EVENT
			CTRL_LOGOFF_EVENT = 5
			CTRL_SHUTDOWN_EVENT
		End Enum

		' A private static handler function in the MyApp class.
		Public Shared Function Handler(ByVal CtrlType As MyWin32.CtrlTypes) As Boolean

			Dim message As String = "This message should never be seen!"

			' A switch to handle the event type.
			Select Case CtrlType
				Case MyWin32.CtrlTypes.CTRL_C_EVENT, MyWin32.CtrlTypes.CTRL_BREAK_EVENT
					message = "stop execution, since CTRL command!"
					Console.m_endEvent.Set()
					' break
				Case MyWin32.CtrlTypes.CTRL_CLOSE_EVENT
					message = "A CTRL_CLOSE_EVENT was raised by the user."
					' break
				Case MyWin32.CtrlTypes.CTRL_LOGOFF_EVENT
					message = "A CTRL_LOGOFF_EVENT was raised by the user."
					' break
				Case MyWin32.CtrlTypes.CTRL_SHUTDOWN_EVENT
					message = "A CTRL_SHUTDOWN_EVENT was raised by the user."
					' break
			End Select

			' Use interop to display a message for the type of event.
			System.Console.WriteLine(message)

			Return True
		End Function
	End Class
#End Region

#Region " //  Public Toolkit Derived classes"

	Public Class BISCreator
		Inherits Creator


		Public Overloads Overrides Function CreateTransaction(ByVal aTransactionType As EnumTransactionType, ByVal aRequests As DaRequest(), ByVal aSessionKey As UInt32) As DaTransaction
			Return CType(New BISTransaction(aTransactionType, aRequests, aSessionKey), DaTransaction)
		End Function

		Public Overloads Overrides Function CreateDaAddressSpaceRoot() As DaAddressSpaceRoot
			Return New BISDaRoot
		End Function


		Public Overloads Overrides Function CreateAeAddressSpaceRoot() As AeAddressSpaceRoot
			Return New BISAeRoot
		End Function

	End Class


	Public Class BISDaRoot
		Inherits DaAddressSpaceRoot


		Public Overloads Overrides Function QueryAddressSpaceElementData(ByVal anElementId As String, ByRef anElement As AddressSpaceElement) As System.Int32

			anElement = Nothing
			Dim elements As Hashtable = Hashtable.Synchronized(Me.m_elements)
			For Each daElement As DaAddressSpaceElement In elements.Values
				If daElement.ItemId = anElementId Then
					anElement = daElement
					' break
				End If
			Next

			Return Convert.ToInt32(EnumResultCode.S_OK)

		End Function


		Public Overloads Overrides Function QueryAddressSpaceElementChildren(ByVal anElementId As String, ByVal aChildrenList As ArrayList) As System.Int32

			Dim elements As Hashtable = Hashtable.Synchronized(Me.m_elements)
			For Each daElement As DaAddressSpaceElement In elements.Values
				If daElement.ItemId = anElementId Then
					aChildrenList.InsertRange(0, daElement.GetChildren)
					' break
				End If
			Next

			Return Convert.ToInt32(EnumResultCode.S_OK)

		End Function


	End Class


	Public Class BISAeRoot
		Inherits AeAddressSpaceRoot


		Public Overloads Overrides Function QueryAddressSpaceElementData(ByVal anElementId As String, ByRef anElement As AddressSpaceElement) As System.Int32

			anElement = Nothing
			Dim elements As Hashtable = Hashtable.Synchronized(Me.m_elements)
			For Each aeElement As AeAddressSpaceElement In elements.Values
				If aeElement.Name = anElementId Then
					anElement = aeElement
					Return Convert.ToInt32(EnumResultCode.S_OK)
				End If
			Next

			Return Convert.ToInt32(EnumResultCode.S_FALSE)

		End Function


		Public Overloads Overrides Function QueryAddressSpaceElementChildren(ByVal anElementId As String, ByVal aChildrenList As ArrayList) As System.Int32

			Dim elements As Hashtable = Hashtable.Synchronized(Me.m_elements)
			For Each aeElement As AeAddressSpaceElement In elements.Values
				If aeElement.Name = anElementId Then
					aChildrenList.InsertRange(0, aeElement.GetChildren)
					' break
				End If
			Next

			Return Convert.ToInt32(EnumResultCode.S_OK)

		End Function

	End Class


	Public Class BISTransaction
		Inherits DaTransaction


		Public Sub New(ByVal aTransactionType As EnumTransactionType, ByVal aRequests As DaRequest(), ByVal aSessionKey As UInt32)
			MyBase.New(aTransactionType, aRequests, aSessionKey)
		End Sub

		Public Overloads Overrides Function HandleReadRequests() As System.Int32

			While Requests.Count > 0
				Dim request As DaRequest = CType(Requests(0), DaRequest)
				If Not (request Is Nothing) Then
					If request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) Then
						Dim element As DaBaseElement = CType(request.AddressSpaceElement, DaBaseElement)
						If Not (element Is Nothing) Then
							element.HandleReadRequest(request)
						Else
							request.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.E_FAIL)
							request.Value = Nothing
							request.Complete()
						End If
					End If
				End If

			End While

			Return Convert.ToInt32(EnumResultCode.S_OK)

		End Function


		Public Overloads Overrides Function HandleWriteRequests() As System.Int32

			While Requests.Count > 0

				Dim request As DaRequest = CType(Requests(0), DaRequest)
				If Not (request Is Nothing) Then
					If request.ProgressRequestState(EnumRequestState.CREATED, EnumRequestState.INPROGRESS) Then
						Dim element As DaBaseElement = CType(request.AddressSpaceElement, DaBaseElement)

						If Not (element Is Nothing) Then
							element.HandleWriteRequest(request)
						Else
							request.Result = [Enum].ToObject(GetType(EnumResultCode), EnumResultCode.E_FAIL)
							request.Complete()
						End If
					End If
				End If
			End While

			Return Convert.ToInt32(EnumResultCode.S_OK)
		End Function

	End Class

#End Region

#Region " //  Main"

	Class Console

		Public Shared m_endEvent As AutoResetEvent
		Public Shared device1Node As DaBaseElement = Nothing
		Public Shared device2Node As DaBaseElement = Nothing

		Shared Sub initServer()

			Application.Instance.VersionOtb = 447
			Application.Instance.AppType = EnumApplicationType.EXECUTABLE

			Application.Instance.ClsidDa = "{5B39F326-474C-42C2-B0E0-C7FEC2848D1A}"
			Application.Instance.ProgIdDa = "Softing.OPC.Toolkit.NET.VB.Demo.BIS.DA.1"
			Application.Instance.VerIndProgIdDa = "Softing.OPC.Toolkit.NET.VB.Demo.BIS.DA"

			Application.Instance.ClsidAe = "{B74BC40B-9055-42C1-80CF-7D598A2A8C0B}"
			Application.Instance.ProgIdAe = "Softing.OPC.Toolkit.NET.VB.Demo.BIS.AE.1"
			Application.Instance.VerIndProgIdAe = "Softing.OPC.Toolkit.NET.VB.Demo.BIS.AE"

			Application.Instance.Description = "Softing OPC Toolkit .NET VB BIS Demo Server"
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
			Application.Instance.AddressSpaceDelimiter = "."c
			Application.Instance.PropertyDelimiter = "#"c
			Application.Instance.SupportDisableConditions = False
			AddHandler Application.Instance.ShutdownRequest, AddressOf Shutdown
			Dim traceLevelAll As EnumTraceGroup = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL)
			Dim traceLevelServer As EnumTraceGroup = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.SERVER)

			Application.Instance.EnableTracing( _
			  traceLevelAll, _
			  traceLevelAll, _
			  traceLevelServer, _
			  traceLevelServer, _
			  "Trace.txt", _
			  System.Convert.ToUInt32(1000000), _
			  System.Convert.ToUInt32(0))

		End Sub

		''' <summary>
		''' The main entry point for the application.
		''' </summary>
		Shared Sub Main(ByVal args As String())

			Try

				m_endEvent = New AutoResetEvent(False)

				Dim hr As MyWin32.HandlerRoutine = Nothing
				hr = AddressOf MyWin32.Handler

				MyWin32.SetConsoleCtrlHandler(hr, True)

				initServer()

				If ResultCode.SUCCEEDED(Application.Instance.Initialize(New BISCreator)) Then

					Dim ret As Integer = Convert.ToInt32(Application.Instance.ProcessCommandLine(Environment.CommandLine))

					If ret = Convert.ToInt32(EnumResultCode.S_OK) Then

						If ResultCode.SUCCEEDED(Application.Instance.Start) Then

							BuildEventCategories()
							BuildNamespace()
							Application.Instance.Ready()
							System.Console.WriteLine("Press Ctrl-C to exit")

							Dim bEnd As Boolean = False
							Dim simulationIndex As Integer = 0
							Dim rand As Random = New Random
							While Not bEnd

								' wait for keyboard entry 'Ctrl + C' or 'Ctrl + Break'
								If (bEnd = m_endEvent.WaitOne(100, False)) = True Then
									'continue
								End If
								If simulationIndex Mod 10 = 0 Then
									device1Node.Simulate()
								End If

								If simulationIndex Mod 25 = 0 Then
									device2Node.Simulate()
								End If

								simulationIndex += 1

							End While
							Application.Instance.Stop()
						End If
					Else
						If ret = Convert.ToInt32(EnumResultCode.S_FALSE) Then
							System.Console.WriteLine("Registration operation {0}", Environment.CommandLine)
						End If
					End If
					Application.Instance.Terminate()
				End If
			Catch e As Exception
				Application.Instance.Trace(EnumTraceLevel.ERR, [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.OPCSERVER), "console.main", "Exception" + e.ToString)
			End Try
		End Sub


		Public Shared Sub BuildEventCategories()

			'  we have a couple of events. Each class is responsible for building it's own cathegory
			DataEvent.BuildCategory()
			DeviceEvent.BuildCategory()
		End Sub


		Public Shared Sub BuildNamespace()

			device1Node = New DaBaseElement
			device1Node.HasChildren = True
			device1Node.Name = "BMS1"
			Application.Instance.DaAddressSpaceRoot.AddChild(device1Node)

			device1Node.AddChild(New DataPointElement("Datapoint11", 5003, "Meeting room 1"))
			device1Node.AddChild(New DataPointElement("Datapoint12", 5004, "Office 1"))
			device1Node.AddChild(New DataPointElement("Datapoint13", 5004, "Office 2"))
			device1Node.AddChild(New DataPointElement("Datapoint14", 5003, "Office 3"))
			device1Node.AddChild(New DataPointElement("Datapoint15", 5003, "Office 4"))
			device1Node.AddChild(New DataPointElement("Datapoint16", 5004, "Office 5"))
			device1Node.AddChild(New DeviceElement("DeviceState", 5002, "Device connection status datapoint"))

			device2Node = New DaBaseElement
			device2Node.HasChildren = True
			device2Node.Name = "BMS2"
			Application.Instance.DaAddressSpaceRoot.AddChild(device2Node)

			device2Node.AddChild(New DataPointElement("Datapoint21", 5005, "Meeting room 1"))
			device2Node.AddChild(New DataPointElement("Datapoint22", 5005, "Office 1"))
			device2Node.AddChild(New DataPointElement("Datapoint23", 5005, "Office 2"))
			device2Node.AddChild(New DataPointElement("Datapoint24", 5005, "Office 3"))
			device2Node.AddChild(New DataPointElement("Datapoint25", 5006, "Office 4"))
			device2Node.AddChild(New DataPointElement("Datapoint26", 5006, "Office 5"))
			device2Node.AddChild(New DataPointElement("Datapoint27", 5005, "Office 5"))
			device2Node.AddChild(New DeviceElement("DeviceState", 5002, "Device connection status datapoint"))

			Dim cmd As CommandElement = New CommandElement

			Dim cmdProperty As CommandProperty = New CommandProperty(5002, "Device State")
			cmdProperty.AddCommand("1", "Initialises Comms")
			cmdProperty.AddCommand("0", "Closes Comms")
			cmd.AddProperty(cmdProperty)

			cmdProperty = New CommandProperty(5003, "BMS1 5003")
			cmdProperty.AddCommand("0", "Init")
			cmdProperty.AddCommand("1", "Close")
			cmd.AddProperty(cmdProperty)

			cmdProperty = New CommandProperty(5004, "BMS1 5004")
			cmdProperty.AddCommand("2", "Resets sensor")
			cmdProperty.AddCommand("4", "Activates the sensor")
			cmd.AddProperty(cmdProperty)

			cmdProperty = New CommandProperty(5005, "BMS2 5005")
			cmdProperty.AddCommand("1", "Activate Zone")
			cmdProperty.AddCommand("3", "Deactivate Zone")
			cmd.AddProperty(cmdProperty)

			cmdProperty = New CommandProperty(5006, "BMS1 5006")
			cmdProperty.AddCommand("1", "Heating on")
			cmdProperty.AddCommand("0", "Heating off")
			cmd.AddProperty(cmdProperty)

			Application.Instance.DaAddressSpaceRoot.AddChild(cmd)

			'  Trigger AE initialization for each device
			device1Node.InitAe(Application.Instance.AeAddressSpaceRoot)
			device2Node.InitAe(Application.Instance.AeAddressSpaceRoot)

		End Sub


		Public Shared Function Shutdown() As System.Int32
			m_endEvent.Set()
			System.Console.WriteLine("Shutodwn indication received", Environment.CommandLine)
			Return Convert.ToInt32(EnumResultCode.S_OK)
		End Function
	End Class

#End Region

End Namespace

