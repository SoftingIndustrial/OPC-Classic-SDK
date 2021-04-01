Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DaConsole
	Public Class MyDaSubscription
		Inherits DaSubscription

#Region "Constructor"
		'-----------------		
		Public Sub New(ByVal updateRate As UInt32, ByVal parentSession As MyDaSession)
			MyBase.New(updateRate, parentSession)
			AddHandler DataChanged, AddressOf HandleDataChanged
			AddHandler StateChangeCompleted, AddressOf HandleStateChanged
			AddHandler ReadCompleted, AddressOf HandleSubscriptionReadCompleted
			AddHandler WriteCompleted, AddressOf HandleSubscriptionWriteCompleted
			AddHandler PerformStateTransitionCompleted, AddressOf HandlePerformStateTransition
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

		Public Shared Sub HandleStateChanged(ByVal sender As ObjectSpaceElement, ByVal state As EnumObjectState)
			System.Console.WriteLine(sender.ToString() + " State Changed - " + state.ToString())
		End Sub


		Public Shared Sub HandleDataChanged(ByVal daSubscription As DaSubscription, ByVal items As DaItem(), ByVal values As ValueQT(), ByVal results As Integer())
			System.Console.WriteLine()
			System.Console.WriteLine(daSubscription.ToString())
			Dim i As Integer = 0
			While i < items.Length
				If ResultCode.SUCCEEDED(results(i)) Then
					System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", items(i).Id, "-", values(i).ToString()))
				Else
					System.Console.WriteLine("Receiving item value failed! " + " Item: " + items(i).Id)
				End If
				System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
			End While
			System.Console.WriteLine()
		End Sub


		Public Shared Sub HandleSubscriptionReadCompleted(ByVal daSubscription As DaSubscription, ByVal executionContext As UInt32, ByVal items As DaItem(), ByVal values As ValueQT(), ByVal results As Integer(), ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine(daSubscription.ToString() + " asynchronous read succeeded! ")

				Dim i As Integer = 0
				While i < values.Length
					If ResultCode.SUCCEEDED(results(i)) Then
						System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", items(i).Id, "-", values(i).ToString()))
					Else
						System.Console.WriteLine(" Item read failed!" + items(i).Id + " [ResultID: " + results(i).ToString() + " ]")
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else
				System.Console.WriteLine(" Subscription asynchronous read failed! Result: " + result.ToString())
			End If
		End Sub


		Public Shared Sub HandleSubscriptionWriteCompleted(ByVal daSubscription As DaSubscription, ByVal executionContext As UInt32, ByVal items As DaItem(), ByVal values As ValueQT(), ByVal results As Integer(), ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine(daSubscription.ToString() + " asynchronous write succeeded! ")
				Dim i As Integer = 0
				While i < items.Length
					If ResultCode.SUCCEEDED(results(i)) Then
						System.Console.WriteLine(" " + items(i).Id)
					Else
						System.Console.WriteLine(" aSubscription asynchronous write failed! " + items(i).Id + " [ResultID: " + results(i).ToString() + " ]")
					End If
					System.Math.Max(System.Threading.Interlocked.Increment(i), i - 1)
				End While
			Else
				System.Console.WriteLine(" Subscription asynchronous write failed! Result: " + result.ToString())
			End If
		End Sub


		Public Shared Sub HandlePerformStateTransition(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				System.Console.WriteLine(sender.ToString() + " Performed state transition - " + executionContext.ToString())
			Else
				System.Console.WriteLine(sender.ToString() + "  Performed state transition failed! Result: " + result.ToString())
			End If
		End Sub


		'--
#End Region

	End Class

End Namespace

