Imports System
Imports Softing.OPCToolbox.Client
Imports Softing.OPCToolbox

Namespace DaConsole
	Public Class MyDaItem
		Inherits DaItem

#Region "Constructor"
		'-----------------

		Public Sub New(ByVal itemId As String, ByVal parentSubscription As MyDaSubscription)
			MyBase.New(itemId, parentSubscription)
			AddHandler ValueChanged, AddressOf HandleValueChanged
			AddHandler StateChangeCompleted, AddressOf HandleStateChanged
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
			Dim item As MyDaItem = DirectCast(sender, MyDaItem)
			System.Console.WriteLine(item.ToString() + " " + item.Id + " State Changed - " + state.ToString())
		End Sub


		Public Shared Sub HandleValueChanged(ByVal aDaItem As DaItem, ByVal aValue As ValueQT)
			If aDaItem.Id = "maths.sin" Then
				System.Console.WriteLine("Value changed!")
				System.Console.WriteLine([String].Format("{0,-19} {1} {2,-50} ", aDaItem.Id, "-", aValue.ToString()))
			End If
		End Sub


		Public Shared Sub HandlePerformStateTransition(ByVal sender As ObjectSpaceElement, ByVal executionContext As UInt32, ByVal result As Integer)
			If ResultCode.SUCCEEDED(result) Then
				Dim item As MyDaItem = DirectCast(sender, MyDaItem)
				System.Console.WriteLine(sender.ToString() + " " + item.Id + " Performed state transition - " + executionContext.ToString())
			Else
				System.Console.WriteLine(sender.ToString() + "  Performed state transition failed! Result: " + result.ToString())
			End If
		End Sub


		'--
#End Region

	End Class


End Namespace

