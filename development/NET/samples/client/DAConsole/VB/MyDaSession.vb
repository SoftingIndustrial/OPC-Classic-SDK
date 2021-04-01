Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DaConsole
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
			AddHandler LogonCompleted, AddressOf HandleLogonCompleted
			AddHandler LogoffCompleted, AddressOf HandleLogoffCompleted
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
			System.Console.WriteLine(sender.ToString() + " State Changed - " + state.ToString())
		End Sub


		Public Shared Sub HandlePerformStateTransition(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine(sender.ToString() + " Performed state transition - " + executionContext.ToString())
			Else
				System.Console.WriteLine(sender.ToString() + "  Performed state transition failed! Result: " + result.ToString())
			End If
		End Sub


		Public Shared Function HandleServerShutdown(ByVal sender As ObjectSpaceElement, ByVal reason As String) As Boolean
			System.Console.WriteLine()
			System.Console.WriteLine(sender.ToString() + " Shutdown - " + reason)
			System.Console.WriteLine()
			Return True
		End Function


		Public Shared Sub HandleSessionReadCompleted(ByVal daSession As DaSession, ByVal executionContext As UInt32, ByVal itemIds As String(), ByVal itemPaths As String(), ByVal values As ValueQT(), ByVal results As Integer(), _
		ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine(daSession.ToString() + " asynchronous read succeeded! ")
				Dim i As Integer = 0
				While i < itemIds.Length
					If ResultCode.SUCCEEDED(results(i)) Then
						System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", itemIds(i).ToString(), "-", values(i).ToString()))
					Else
						System.Console.WriteLine(" Session asynchronous read failed for item " + " Item: " + itemIds(i).ToString() + " [ResultID: " + results(i).ToString() + " ]")
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else
				System.Console.WriteLine(" Session asynchronous read failed! Result: " + result.ToString())
			End If
		End Sub


		Public Shared Sub HandleSessionWriteCompleted(ByVal daSession As DaSession, ByVal executionContext As UInt32, ByVal itemIds As String(), ByVal itemPaths As String(), ByVal values As ValueQT(), ByVal results As Integer(), _
		ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine(daSession.ToString() + " asynchronous write succeeded! ")
				Dim i As Integer = 0
				While i < itemIds.Length
					If ResultCode.SUCCEEDED(results(i)) Then
						System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", itemIds(i).ToString(), "-", values(i).ToString()))
					Else
						System.Console.WriteLine(" Session asynchronous write for item " + itemIds(i).ToString() + " [ResultID: " + results(i).ToString() + " ]")
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else
				System.Console.WriteLine("Session asynchronous write failed! Result:" + result.ToString())
			End If
		End Sub


		Public Shared Sub HandleGetServerStatus(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal serverStatus As ServerStatus, ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine(sender.ToString())
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
					System.Console.WriteLine("  Supported LCID: " + serverStatus.SupportedLcIds(i).ToString())
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
				System.Console.WriteLine("  Status info: " + serverStatus.StatusInfo)
			Else
				System.Console.WriteLine("Asynchronous get server status failed! Result: " + result.ToString())
			End If
		End Sub

		Public Shared Sub HandleLogonCompleted(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal username As String, ByVal password As String, ByVal result As Integer)
			System.Console.Write("Logon for secure communication")
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine("- successfull logon for user:" + username)
			Else
				System.Console.WriteLine("- failed logon:" + result.ToString)
			End If
		End Sub

		Public Shared Sub HandleLogoffCompleted(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal result As Integer)
			System.Console.Write("Logoff for secure communication")
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine("- successfull logoff")
			Else
				System.Console.WriteLine("- failed logoff:" + result.ToString)
			End If
		End Sub
		'--
#End Region

	End Class

End Namespace

