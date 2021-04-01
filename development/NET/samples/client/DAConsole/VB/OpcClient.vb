Imports System
Imports Microsoft.VisualBasic
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DaConsole
	Public Class OpcClient

#Region "Fields"
		Dim m_qualityNotSet As EnumQuality
		Dim m_traceGroupUser As EnumTraceGroup
#End Region

#Region "Constructor"
		'-----------------

		Public Sub New()
			m_qualityNotSet = [Enum].ToObject(GetType(EnumQuality), EnumQuality.QUALITY_NOT_SET)
			m_traceGroupUser = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.USER)
		End Sub

		'--
#End Region

#Region "Private Members"
		'---------------------
		Private m_usr As String
		Private m_pwd As String

#Region "Private Attributes"
		'------------------------

		Private m_daSession As MyDaSession = Nothing
		Private m_daSubscription As MyDaSubscription = Nothing
		Private m_daItem1 As MyDaItem = Nothing
		Private m_daItem2 As MyDaItem = Nothing
		Private m_daItem3 As MyDaItem = Nothing
		Private m_daItem4 As MyDaItem = Nothing
		Private m_daItem5 As MyDaItem = Nothing
		Private m_itemList As MyDaItem()
		Private m_itemIds As String()
		Private m_results As Integer()
		Private m_values As ValueQT()
		Private m_executionOptions As ExecutionOptions
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

			Dim result As Integer = EnumResultCode.S_OK
			GetApplication().VersionOtb = 447

			'  enable toolkit traces

			Dim traceGroupAll As EnumTraceGroup
			Dim traceGroupClient As EnumTraceGroup

			traceGroupClient = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.CLIENT)
			traceGroupAll = [Enum].ToObject(GetType(EnumTraceGroup), EnumTraceGroup.ALL)

			GetApplication().EnableTracing( _
			 traceGroupAll, _
			 traceGroupAll, _
			 traceGroupClient, _
			 traceGroupClient, _
			 "Trace.txt", _
			 Convert.ToUInt32(1000000), _
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
			m_daSubscription.RemoveDaItem(m_daItem4)
			m_daSubscription.RemoveDaItem(m_daItem5)

			m_daSession.RemoveDaSubscription(m_daSubscription)

			GetApplication().RemoveDaSession(m_daSession)

			GetApplication().Terminate()
			m_daSession = Nothing
			m_daSubscription = Nothing
			m_daItem1 = Nothing
			m_daItem2 = Nothing
			m_daItem3 = Nothing
			m_daItem4 = Nothing
			m_daItem5 = Nothing
		End Sub


		Public Function InitializeDaObjects() As Integer

			Dim connectResult As Integer = EnumResultCode.E_FAIL

			m_executionOptions = New ExecutionOptions
			m_executionOptions.ExecutionType = EnumExecutionType.ASYNCHRONOUS
			m_executionOptions.ExecutionContext = Convert.ToUInt32(0)

			m_itemList = New MyDaItem(4) {}
			m_itemIds = New String() {"maths.sin", "time.local.second", "increment.UI1 array", "increment.BSTR", "secure.I2"}

			Try

				'  TODO add your server URL here
				'  this is the server url for Softing OPCToolbox Demo Server
				'  first choice is COM-DA 
				Dim url As String = "opcda:///Softing.OPCToolboxDemo_ServerDA.1/{2E565242-B238-11D3-842D-0008C779D775}"

				'uncomment if you need an XML-DA access
				'  Dim url As String = "http://localhost:8079/OPC/DA"

				m_daSession = New MyDaSession(url)

				If Not m_daSession.Valid Then
					Return connectResult
				End If

				m_daSubscription = New MyDaSubscription(Convert.ToUInt32(1000), m_daSession)

				If Not m_daSubscription.Valid Then
					Return connectResult
				End If

				m_daItem1 = New MyDaItem("maths.sin", m_daSubscription)

				If Not m_daItem1.Valid Then
					Return connectResult
				End If
				m_itemList(0) = m_daItem1

				m_daItem2 = New MyDaItem("time.local.second", m_daSubscription)

				If Not m_daItem2.Valid Then
					Return connectResult
				End If
				m_itemList(1) = m_daItem2

				m_daItem3 = New MyDaItem("increment.UI1 array", m_daSubscription)
				If Not m_daItem3.Valid Then
					Return connectResult
				End If
				m_itemList(2) = m_daItem3

				m_daItem4 = New MyDaItem("increment.BSTR", m_daSubscription)
				If Not m_daItem4.Valid Then
					Return connectResult
				End If

				m_itemList(3) = m_daItem4

				m_daItem5 = New MyDaItem("secure.I2", m_daSubscription)
				If Not m_daItem4.Valid Then
					Return connectResult
				End If

				m_itemList(4) = m_daItem5

				connectResult = m_daSession.Connect(True, False, m_executionOptions)
			Catch exc As Exception
				GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::InitializaDaObjects", exc.ToString())
			End Try

			Return connectResult
		End Function

		Public Function ReadItem() As String
			Dim message As String = [String].Empty

			Try
				Dim readCount As UInt32 = Convert.ToUInt32(0)
				Dim m_itemList As MyDaItem() = New MyDaItem(Convert.ToInt32(readCount)) {}
				m_itemList(0) = m_daItem1

				Dim m_values As ValueQT() = Nothing
				Dim m_results As Integer() = Nothing

				If ResultCode.SUCCEEDED(m_daSubscription.Read(Convert.ToUInt32(0), m_itemList, m_values, m_results, Nothing)) Then
					message &= " " & Chr(10) & "Read item synchronously using subscription " & Chr(10) & ""

					Dim i As Integer = 0
					While i < m_values.Length
						If ResultCode.SUCCEEDED(m_results(i)) Then
							message &= " " + m_itemList(i).Id + " - "
							message += m_values(i).ToString() + "" & Chr(10) & "" & Chr(10) & ""
						Else
							message &= "Read failed for item " + m_itemList(i).Id + "" & Chr(10) & "" & Chr(10) & ""
						End If
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				Else
					message &= " Subscription synchronous read failed!" + "" & Chr(10) & "" & Chr(10) & ""
				End If
			Catch exc As Exception
				GetApplication().Trace(EnumTraceLevel.ERR, m_traceGroupUser, "OpcClient::ReadItem", exc.ToString())
			End Try
			Return message
		End Function


		Public Sub Trace(ByVal traceLevel As EnumTraceLevel, ByVal traceGroup As EnumTraceGroup, ByVal objectID As String, ByVal message As String)
			Application.Instance.Trace(traceLevel, traceGroup, objectID, message)
		End Sub


		Public Sub ActivateSession(ByVal sync As Boolean)
			System.Console.WriteLine()
			If sync Then
				Dim result As Integer = Me.m_daSession.Connect(True, True, New ExecutionOptions)
				If ResultCode.FAILED(result) Then
					System.Console.WriteLine(" Session activate failed!")
				End If
			Else
				m_daSession.Connect(True, True, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(System.Threading.Interlocked.Increment(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1))
			End If
		End Sub


		Public Sub ConnectSession(ByVal sync As Boolean)
			System.Console.WriteLine()
			If sync Then
				Dim result As Integer = m_daSession.Connect(True, False, New ExecutionOptions)
				If ResultCode.FAILED(result) Then
					System.Console.WriteLine(" Session connect failed!")
				End If
			Else
				m_daSession.Connect(True, False, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If
		End Sub


		Public Sub DisconnectSession(ByVal sync As Boolean)
			System.Console.WriteLine()
			If sync Then
				Dim result As Integer = m_daSession.Disconnect(New ExecutionOptions)
				If ResultCode.FAILED(result) Then
					System.Console.WriteLine(" Session disconnect failed!")
				End If
			Else
				m_daSession.Disconnect(m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If

		End Sub


		Public Sub ReadItems(ByVal sync As Boolean)
			Dim itemValues As ValueQT = Nothing
			Dim itemResult As Integer
			If sync Then
				Dim readResult As Integer = m_daItem1.Read(Convert.ToUInt32(100), itemValues, itemResult, New ExecutionOptions)
				If ResultCode.SUCCEEDED(readResult) Then
					System.Console.WriteLine("Item synchronous read succeded")
					System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", m_daItem1.Id, "-", itemValues.ToString()))
				Else
					System.Console.WriteLine("Synchronous item read failed! " + " Item: " + m_daItem2.Id + " Result: " + readResult.ToString())
				End If

				System.Console.WriteLine()
				Dim result As Integer = EnumResultCode.E_FAIL
				result = m_daSubscription.Read(Convert.ToUInt32(100), m_itemList, m_values, m_results, New ExecutionOptions)
				If ResultCode.SUCCEEDED(result) Then
					System.Console.WriteLine(" Subscription synchronous read succeeded")
					Dim i As Integer = 0
					While i < m_itemList.Length
						If m_results(i) >= 0 Then
							System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", m_itemList(i).Id, "-", m_values(i).ToString()))
						Else
							System.Console.WriteLine(" Item read failed! " + " Item: " + m_itemList(i).Id + " [ResultID: " + m_results(i).ToString() + " ]")
						End If
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				Else
					System.Console.WriteLine(" Synchronous subscription read failed! Result: " + result.ToString())
				End If
				System.Console.WriteLine()
			Else

				m_daItem1.Read(Convert.ToUInt32(100), itemValues, itemResult, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)

				m_daSubscription.Read(Convert.ToUInt32(100), m_itemList, m_values, m_results, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If

		End Sub


		Public Sub WriteItems(ByVal sync As Boolean)
			Dim writeDateTime As New DateTime
			If sync Then
				Dim inValues As ValueQT() = New ValueQT(2) {}

				Dim v1 As SByte() = New SByte(4) {}
				v1(0) = Convert.ToSByte(7)
				v1(1) = Convert.ToSByte(8)
				v1(2) = Convert.ToSByte(9)
				v1(3) = Convert.ToSByte(10)
				v1(4) = Convert.ToSByte(11)

				inValues(0) = New ValueQT(v1, m_qualityNotSet, writeDateTime)
				inValues(1) = New ValueQT(v1, m_qualityNotSet, writeDateTime)


				Dim v2 As String = "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ"
				inValues(1) = New ValueQT(v2, m_qualityNotSet, writeDateTime)

				Dim v3 As Integer = 10

				inValues(2) = New ValueQT(v3, m_qualityNotSet, writeDateTime)
				Dim result As Integer = EnumResultCode.E_FAIL

				Dim inItems As MyDaItem() = New MyDaItem(2) {}
				inItems.SetValue(m_daItem3, 0)
				inItems.SetValue(m_daItem4, 1)
				inItems.SetValue(m_daItem5, 2)
				Dim inResults As Integer() = New Integer(1) {}
				result = m_daSubscription.Write(inItems, inValues, inResults, New ExecutionOptions)
				If ResultCode.SUCCEEDED(result) Then

					System.Console.WriteLine(" Subscription synchronous write succeeded")
					Dim i As Integer = 0
					While i < inItems.Length
						If inResults(i) >= 0 Then
							System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", inItems(i).ToString(), "-", inValues(i).ToString()))
							System.Console.WriteLine()
						Else
							System.Console.WriteLine(" Item synchronous write failed!  " + inItems(i).Id + " [ResultID: " + inResults(i).ToString() + " ]")
						End If
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				Else
					System.Console.WriteLine("Subscription synchronous write failed! Result: " + result.ToString())
				End If
			Else
				Dim inValues As ValueQT() = New ValueQT(2) {}

				Dim v1 As SByte() = New SByte(4) {}
				v1(0) = Convert.ToSByte(7)
				v1(1) = Convert.ToSByte(8)
				v1(2) = Convert.ToSByte(9)
				v1(3) = Convert.ToSByte(10)
				v1(4) = Convert.ToSByte(11)
				inValues(0) = New ValueQT(v1, m_qualityNotSet, writeDateTime)

				Dim v2 As String = "SOFTING"
				inValues(1) = New ValueQT(v2, m_qualityNotSet, writeDateTime)

				Dim v3 As Integer
				inValues(2) = New ValueQT(v3, m_qualityNotSet, writeDateTime)

				Dim inItems As MyDaItem() = New MyDaItem(2) {}
				inItems(0) = m_daItem3
				inItems(1) = m_daItem4
				inItems(2) = m_daItem5

				Dim inResults As Integer() = New Integer(2) {}

				m_daSubscription.Write(inItems, inValues, inResults, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If

		End Sub



		Public Sub ReadUsingSession(ByVal sync As Boolean)
			If sync Then
				Dim result As Integer = EnumResultCode.E_FAIL
				' in case of a XML-DA server use a valid item paths array instead of "null"
				result = m_daSession.Read(Convert.ToUInt32(0), m_itemIds, Nothing, m_values, m_results, New ExecutionOptions)
				If ResultCode.SUCCEEDED(result) Then
					System.Console.WriteLine(" Session synchronous read succeeded!")
					Dim i As Integer = 0
					While i < m_itemList.Length
						If m_results(i) >= 0 Then
							System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", m_itemIds(i).ToString(), "-", m_values(i).ToString()))
						Else
							System.Console.WriteLine(" Synchronous read failed for item " + m_itemList(i).Id + " [ResultID: " + m_results(i).ToString() + " ]")
						End If
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				Else
					System.Console.WriteLine(" Session synchronous read failed! Result: " + result.ToString())
				End If
				System.Console.WriteLine()
			Else
				' in case of a XML-DA server use a valid item paths array instead of "null"
				m_daSession.Read(Convert.ToUInt32(0), m_itemIds, Nothing, m_values, m_results, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If

		End Sub


		Public Sub WriteUsingSession(ByVal sync As Boolean)
			Dim writeDateTime As New DateTime
			If sync Then
				Dim inValues As ValueQT() = New ValueQT(2) {}
				Dim inResults As Integer() = New Integer(2) {}
				Dim inItemIds As String() = New String() {"increment.UI1 array", "increment.BSTR", "secure.I2"}
				Dim result As Integer = EnumResultCode.E_FAIL

				Dim v2 As SByte() = New SByte(4) {}
				v2(0) = Convert.ToSByte(7)
				v2(1) = Convert.ToSByte(8)
				v2(2) = Convert.ToSByte(9)
				v2(3) = Convert.ToSByte(10)
				v2(4) = Convert.ToSByte(11)

				inValues(0) = New ValueQT(v2, m_qualityNotSet, writeDateTime)

				Dim v3 As String = "SOFTING"
				inValues(1) = New ValueQT(v3, m_qualityNotSet, writeDateTime)

				Dim v4 As Integer = 16
				inValues(2) = New ValueQT(v4, m_qualityNotSet, writeDateTime)

				' in case of a XML-DA server use a valid item paths array instead of "null"
				result = m_daSession.Write(inItemIds, Nothing, inValues, inResults, New ExecutionOptions)
				If ResultCode.SUCCEEDED(result) Then
					System.Console.WriteLine(" Session synchronous write succeeded")
					Dim i As Integer = 0
					While i < inItemIds.Length
						If inResults(i) >= 0 Then
							System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", inItemIds(i).ToString(), "-", inValues(i).ToString()))
							System.Console.WriteLine()
						Else
							System.Console.WriteLine(" Item synchronous write failed! " + " Item: " + inItemIds(i).ToString() + " [ResultID: " + inResults(i).ToString() + " ]")
						End If
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
				Else
					System.Console.WriteLine(" Session synchronous write failed! Result: " + result.ToString())
				End If
				System.Console.WriteLine()
			Else
				Dim inValues As ValueQT() = New ValueQT(2) {}
				Dim inResults As Integer() = New Integer(2) {}

				Dim inItemIds As String() = New String() {"increment.UI1 array", "increment.BSTR", "secure.I2"}

				Dim v2 As SByte() = New SByte(4) {}
				v2(0) = Convert.ToSByte(7)
				v2(1) = Convert.ToSByte(8)
				v2(2) = Convert.ToSByte(9)
				v2(3) = Convert.ToSByte(10)
				v2(4) = Convert.ToSByte(11)

				inValues(0) = New ValueQT(v2, m_qualityNotSet, writeDateTime)

				Dim v3 As String = "SOFTING"
				inValues(1) = New ValueQT(v3, m_qualityNotSet, writeDateTime)

				Dim v4 As Integer = 16
				inValues(2) = New ValueQT(v4, m_qualityNotSet, writeDateTime)
				' in case of a XML-DA server use a valid item paths array instead of "null"
				m_daSession.Write(inItemIds, Nothing, inValues, inResults, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If

		End Sub


		Public Sub GetServerStatus(ByVal sync As Boolean)
			System.Console.WriteLine()
			Dim newServerStatus As ServerStatus = Nothing
			Dim serverStatus As ServerStatus = Nothing
			If sync Then
				Dim result As Integer = EnumResultCode.E_FAIL

				result = m_daSession.GetStatus(serverStatus, New ExecutionOptions)
				If ResultCode.SUCCEEDED(result) Then

					System.Console.WriteLine("Server Status")
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
						System.Console.WriteLine("  Supported LcId: " + serverStatus.SupportedLcIds(i).ToString())
						System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
					End While
					System.Console.WriteLine("  Status info: " + serverStatus.StatusInfo)
				Else
					System.Console.WriteLine("Synchronous get status failed! Result: " + result.ToString())
				End If
			Else
				m_daSession.GetStatus(newServerStatus, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If

		End Sub
        Public Sub ChangeDataRetrievalMode()
            Dim currendMode As EnumDataRetrievalMode = m_daSubscription.DataRetreivalMode
            Dim nextMode As EnumDataRetrievalMode = EnumDataRetrievalMode.EnumDataRetrievalMode_ASYNCADVISE
            System.Console.WriteLine("Previous data retrieval mode was: ")

            Select currendMode
                Case EnumDataRetrievalMode.EnumDataRetrievalMode_ASYNCADVISE
                    System.Console.WriteLine("Asynchronous Advises.{0}New data retrieval mode was set: Polled Refresh - XML-DA specific.{0}Please reconnect the subscription.", vbCrLf)
                    nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_POLLEDREFRESH
                Case EnumDataRetrievalMode.EnumDataRetrievalMode_POLLEDREFRESH
                    System.Console.WriteLine("Polled Refresh.{0}New data retrieval mode was set: Synchronous reads.{0}Please reconnect the subscription.", vbCrLf)
                    nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_SYNCREAD
                Case EnumDataRetrievalMode.EnumDataRetrievalMode_SYNCREAD
                    System.Console.WriteLine("Synchronous reads.{0}New data retrieval mode was set: Asynchronous Advises.{0}Please reconnect the subscription.", vbCrLf)
                    nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_ASYNCADVISE
                Case Else
                    System.Console.WriteLine("Not Set.{0}New data retrieval mode was set: Asynchronous Advises.{0}Please reconnect the subscription.", vbCrLf)
                    nextMode = EnumDataRetrievalMode.EnumDataRetrievalMode_ASYNCADVISE
            End Select
            m_daSubscription.DataRetreivalMode = nextMode
        End Sub
		Public Sub ActivateConnectionMonitor()
			Dim result As Integer = EnumResultCode.E_FAIL
			result = m_daSession.ActivateConnectionMonitor(True, Convert.ToUInt32(5000), Convert.ToUInt32(0), Convert.ToUInt32(10000), Convert.ToUInt32(300000))
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine()
				System.Console.WriteLine("Activated connection monitor")
				System.Console.WriteLine()
			Else
				System.Console.WriteLine("Activate connection monitor failed! Result " + result.ToString())
			End If
		End Sub


		Public Sub DeactivateConnectionMonitor()
			Dim result As Integer = EnumResultCode.E_FAIL
			result = m_daSession.ActivateConnectionMonitor(False, Convert.ToUInt32(0), Convert.ToUInt32(0), Convert.ToUInt32(0), Convert.ToUInt32(0))
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine()
				System.Console.WriteLine("Deactivated connection monitor")
				System.Console.WriteLine()
			Else
				System.Console.WriteLine("Deactivate connection monitor failed! Result: " + result.ToString())
			End If
		End Sub

		Public Sub LoginSecure(ByVal sync As Boolean)
			Dim result As Integer = EnumResultCode.E_ACCESSDENIED
			If sync Then
				Dim SessionState As EnumObjectState
				SessionState = m_daSession.CurrentState
				If (SessionState <> EnumObjectState.DISCONNECTED) Then

					result = m_daSession.Logon(m_usr, m_pwd, New ExecutionOptions)
					If result Then
						System.Console.WriteLine()
						System.Console.WriteLine("Logon successfull")
					End If
				Else
					System.Console.WriteLine("Logon unsuccesfull - session not connected")
				End If
			Else
				result = m_daSession.Logon(m_usr, m_pwd, m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If
		End Sub

		Public Sub LogoffSecure(ByRef sync As Boolean)
			Dim result As Integer = EnumResultCode.E_ACCESSDENIED
			If sync Then
				Dim SessionState As EnumObjectState
				SessionState = m_daSession.CurrentState
				If (SessionState <> EnumObjectState.DISCONNECTED) Then

					result = m_daSession.Logoff(New ExecutionOptions)
					If result Then
						System.Console.WriteLine()
						System.Console.WriteLine("Logoff successfull")
					End If
				Else
					System.Console.WriteLine("Logoff unsuccesfull - session not connected")
				End If
			Else
				result = m_daSession.Logoff(m_executionOptions)
				m_executionOptions.ExecutionContext = Convert.ToUInt32(Convert.ToInt32(m_executionOptions.ExecutionContext) + 1)
			End If
		End Sub

		Public Sub SetCredentials()
			System.Console.Write("User name:")
			m_usr = System.Console.ReadLine()
			System.Console.Write("Password:")
			m_pwd = System.Console.ReadLine()

		End Sub
		Public Sub SetCredentials(ByVal usr As String, ByVal pwd As String)
			m_usr = usr
			m_pwd = pwd
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

