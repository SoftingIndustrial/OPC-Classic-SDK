Imports System
Imports System.IO
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DaConsole_Service
	Public Class MyDaSession
		Inherits DaSession

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal url As String)
			MyBase.New(url)
			AddHandler StateChangeCompleted, AddressOf HandleStateChanged
			AddHandler PerformStateTransitionCompleted, AddressOf HandlePerformStateTransition
			AddHandler ShutdownRequest, AddressOf HandleServerShutdown
			AddHandler ReadCompleted, AddressOf HandleSessionReadCompleted
			AddHandler WriteCompleted, AddressOf HandleSessionWriteCompleted
			AddHandler GetStatusCompleted, AddressOf HandleGetServerStatus
		End Sub

		'--
#End Region

#Region "Private Members"
		'---------------------

#Region "Private Attributes"
		'------------------------


		'--
#End Region

		'--
#End Region

#Region "Public Methods"
		'---------------------

		'--
#End Region

#Region "Public Properties"
		'-----------------------


		'--
#End Region

#Region "Handles"
		'---------------------

		Public Shared Sub HandleStateChanged(ByVal sender As ObjectSpaceElement, ByVal state As EnumObjectState)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			streamWriter.WriteLine(sender.ToString() + " State Changed - " + state.ToString())
			streamWriter.Flush()
			streamWriter.Close()
		End Sub


		Public Shared Sub HandlePerformStateTransition(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal result As Integer)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			If ResultCode.SUCCEEDED(result) Then
				streamWriter.WriteLine(sender.ToString() + " Performed state transition - " + executionContext.ToString())
			Else
				streamWriter.WriteLine(sender.ToString() + "  Performed state transition failed! Result: " + result.ToString())
			End If
			streamWriter.Flush()
			streamWriter.Close()
		End Sub


		Public Shared Function HandleServerShutdown(ByVal sender As ObjectSpaceElement, ByVal reason As String) As Boolean
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			streamWriter.WriteLine()
			streamWriter.WriteLine(sender.ToString() + " Shutdown - " + reason.ToString())
			streamWriter.WriteLine()
			streamWriter.Flush()
			streamWriter.Close()
			Return True
		End Function


		Public Shared Sub HandleSessionReadCompleted(ByVal daSession As DaSession, ByVal executionContext As UInt32, ByVal itemIds As String(), ByVal itemPaths As String(), ByVal values As ValueQT(), ByVal results As Integer(), _
		  ByVal result As Integer)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			If ResultCode.SUCCEEDED(result) Then
				streamWriter.WriteLine(daSession.ToString() + " asynchronous read succeeded! ")
				Dim i As Integer = 0
				While i < itemIds.Length
					If ResultCode.SUCCEEDED(results(i)) Then
						streamWriter.WriteLine([String].Format("{0,-19} {1} {2,-50} ", itemIds(i), "-", values(i).ToString()))
					Else
						streamWriter.WriteLine(" Session asynchronous read failed for item " + " Item: " + itemIds(i).ToString() + " [ResultID: " + results(i).ToString() + " ]")
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else
				streamWriter.WriteLine(" Session asynchronous read failed! Result: " + result.ToString())
			End If
			streamWriter.Flush()
			streamWriter.Close()
		End Sub


		Public Shared Sub HandleSessionWriteCompleted(ByVal daSession As DaSession, ByVal executionContext As UInt32, ByVal itemIds As String(), ByVal itemPaths As String(), ByVal values As ValueQT(), ByVal results As Integer(), _
		  ByVal result As Integer)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			If ResultCode.SUCCEEDED(result) Then
				streamWriter.WriteLine(daSession.ToString() + " asynchronous write succeeded! ")
				Dim i As Integer = 0
				While i < itemIds.Length
					If ResultCode.SUCCEEDED(results(i)) Then
						streamWriter.WriteLine([String].Format("{0,-19} {1} {2,-50} ", itemIds(i), "-", values(i).ToString()))
					Else
						streamWriter.WriteLine(" Session asynchronous write for item " + itemIds(i) + " [ResultID: " + results(i).ToString() + " ]")
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else
				streamWriter.WriteLine("Session asynchronous write failed! Result:" + result.ToString())
			End If
			streamWriter.Flush()
			streamWriter.Close()
		End Sub


		Public Shared Sub HandleGetServerStatus(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal serverStatus As ServerStatus, ByVal result As Integer)
			Dim fs As New FileStream("C:\ClientService.txt", FileMode.OpenOrCreate, FileAccess.Write)
			Dim streamWriter As New StreamWriter(fs)
			streamWriter.BaseStream.Seek(0, SeekOrigin.[End])
			If ResultCode.SUCCEEDED(result) Then
				streamWriter.WriteLine(sender.ToString())
				streamWriter.WriteLine("Server Status")
				streamWriter.WriteLine("  Vendor info: " + serverStatus.VendorInfo)
				streamWriter.WriteLine("  Product version: " + serverStatus.ProductVersion)
				streamWriter.WriteLine("  State: " + serverStatus.State.ToString())
				streamWriter.WriteLine("  Start time: " + serverStatus.StartTime.ToString())
				streamWriter.WriteLine("  Last update time: " + serverStatus.LastUpdateTime.ToString())
				streamWriter.WriteLine("  Current time: " + serverStatus.CurrentTime.ToString())
				streamWriter.WriteLine("  GroupCount: " + serverStatus.GroupCount.ToString())
				streamWriter.WriteLine("  Bandwidth: " + serverStatus.Bandwidth.ToString())
				Dim i As Integer = 0
				While i < serverStatus.SupportedLcIds.Length
					streamWriter.WriteLine("  Supported LCID: " + serverStatus.SupportedLcIds(i))
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
				streamWriter.WriteLine("  Status info: " + serverStatus.StatusInfo)
			Else
				streamWriter.WriteLine("Asynchronous get server status failed! Result: " + result.ToString())
			End If
			streamWriter.Flush()
			streamWriter.Close()
		End Sub

		'--
#End Region

	End Class

End Namespace

