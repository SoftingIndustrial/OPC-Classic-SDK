Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace AEConsole
	Public Class MyAeSession
		Inherits AeSession

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal url As String)
			MyBase.New(url)
			AddHandler StateChangeCompleted, AddressOf HandleStateChangeCompleted
			AddHandler PerformStateTransitionCompleted, AddressOf HandlePerformObjectStateTransitionCompleted
			AddHandler ShutdownRequest, AddressOf HandleShutdown
			AddHandler GetServerStatusCompleted, AddressOf HandleGetStatusCompleted
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

		' method that handles the completion of performing the ObjectSpaceElement state transition
		Public Shared Sub HandlePerformObjectStateTransitionCompleted(ByVal obj As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine(obj.ToString() + " Performed state transition " + executionContext.ToString())
			Else
				System.Console.WriteLine(obj.ToString() + " Performed state transition failed")
			End If
		End Sub

		'method that handles the session shutdown
		Public Shared Function HandleShutdown(ByVal obj As ObjectSpaceElement, ByVal reason As String) As Boolean
			System.Console.WriteLine()
			System.Console.WriteLine(obj.ToString() + " Shutdown - " + reason)
			System.Console.WriteLine()
			Return True
		End Function

		' method that handles the completion of state changing of an ObjectSpaceElement
		Public Shared Sub HandleStateChangeCompleted(ByVal obj As ObjectSpaceElement, ByVal state As EnumObjectState)
			System.Console.WriteLine(obj.ToString() + " State Changed - " + state.ToString())
		End Sub

		' method that handles the completion of getting the server status
		Public Shared Sub HandleGetStatusCompleted(ByVal obj As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal serverStatus As ServerStatus, ByVal result As Integer)
			If ResultCode.FAILED(result) Then
				System.Console.WriteLine(obj.ToString() + "Asynchronous GetServerStatus Failed")
			End If
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine("Asynchronous GetServerStatus Succeeded")
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
					System.Console.WriteLine("  Supported LCID: " + serverStatus.SupportedLcIds(i))
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
				System.Console.WriteLine("  Status info: " + serverStatus.StatusInfo)
			End If
		End Sub

		'--
#End Region

	End Class

End Namespace

